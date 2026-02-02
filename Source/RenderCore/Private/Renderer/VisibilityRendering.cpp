#include "VisibilityRendering.h"

#include <numeric>

#include "GlobalShaders.h"
#include "RenderGraph.h"

namespace rendercore
{
    class InitCounterCS final : public GlobalShaderCommon<ComputeShader,  InitCounterCS>
    {
    private:
        DEFINE_SHADER_PARAM( Counter );
        DEFINE_SHADER_PARAM( IndirectArgs );

        DEFINE_SHADER_PARAM( NumDrawCallIds );
    };

    class CountDrawCallIdCS final : public GlobalShaderCommon<ComputeShader, CountDrawCallIdCS>
    {
    private:
        DEFINE_SHADER_PARAM( Visibility );
        DEFINE_SHADER_PARAM( Counter );

        DEFINE_SHADER_PARAM( ScreenSize );
    };

    class InitPrefixSumBufferCS final : public GlobalShaderCommon<ComputeShader, InitPrefixSumBufferCS>
    {
    private:
        DEFINE_SHADER_PARAM( BlockId );
        DEFINE_SHADER_PARAM( BlockStatus );

        DEFINE_SHADER_PARAM( NumBlocks );
    };

    class PrefixSumCS final : public GlobalShaderCommon<ComputeShader, PrefixSumCS>
    {
        DEFINE_SHADER_PARAM( Input );

        DEFINE_SHADER_PARAM( BlockId );
        DEFINE_SHADER_PARAM( BlockStatus );

        DEFINE_SHADER_PARAM( Output );

        DEFINE_SHADER_PARAM( NumItems );
    };

    class BuildWorkListCS final : public GlobalShaderCommon<ComputeShader, BuildWorkListCS>
    {
        DEFINE_SHADER_PARAM( Visibility );
        DEFINE_SHADER_PARAM( Offset );

        DEFINE_SHADER_PARAM( IndirectArgs );
        DEFINE_SHADER_PARAM( WorkList );

        DEFINE_SHADER_PARAM( ScreenSize );
    };

    class FinalizeIndirectArgsCS final : public GlobalShaderCommon<ComputeShader, FinalizeIndirectArgsCS>
    {
        DEFINE_SHADER_PARAM( IndirectArgs );

        DEFINE_SHADER_PARAM( NumDrawCallIds );
    };

    REGISTER_GLOBAL_SHADER( InitCounterCS, "./Assets/Shaders/Visibility/CS_InitCounter.asset" );
    REGISTER_GLOBAL_SHADER( CountDrawCallIdCS, "./Assets/Shaders/Visibility/CS_CountDrawCallId.asset" );
    REGISTER_GLOBAL_SHADER( InitPrefixSumBufferCS, "./Assets/Shaders/Visibility/CS_InitPrefixSumBuffer.asset" );
    REGISTER_GLOBAL_SHADER( PrefixSumCS, "./Assets/Shaders/Visibility/CS_PrefixSum.asset" );
    REGISTER_GLOBAL_SHADER( BuildWorkListCS, "./Assets/Shaders/Visibility/CS_BuildWorkList.asset" );
    REGISTER_GLOBAL_SHADER( FinalizeIndirectArgsCS, "./Assets/Shaders/Visibility/CS_FinalizeIndirectArgs.asset" );

    void VisibilityBuffer::RenderBuffer( RenderGraph& renderGraph, const RenderBufferParam& param )
    {
        if ( param.IsValid() == false )
        {
            return;
        }

        GPU_PROFILE_EVENT( renderGraph, RenderBuffer )

        ResourceBinder& resourceBinder = param.m_resourceBinder;
        RenderFrameArray<VisibleDrawSnapshot>& visibilityPassSnapshots = param.m_visibilityPassData.m_visibilityPassSnapshots;
        if ( visibilityPassSnapshots.size() == 0 )
        {
            return;
        }

        // Copy draw call IDs
        auto numSnapshots = static_cast<uint32>( visibilityPassSnapshots.size() );

        agl::BufferTrait uploaderTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = numSnapshots,
            .m_access = agl::ResourceAccess::Upload,
            .m_bindType = agl::ResourceBindType::None,
            .m_miscFlag = agl::ResourceMisc::Intermediate,
            .m_format = agl::ResourceFormat::Unknown
        };

        RefHandle<agl::Buffer> uploadBuffer = GraphicsResourcePool::GetInstance().FindFreeBuffer( uploaderTrait, "Visibility.UploadBuffer" );

        if ( auto buffer = GraphicsInterface().Lock<uint32>( uploadBuffer.Get() ) )
        {
            for ( uint32 id : param.m_visibilityPassData.m_shadingSnapshotIds )
            {
                *( buffer++ ) = id;
            }

            GraphicsInterface().UnLock( uploadBuffer.Get() );
        }

        agl::BufferTrait drawCallIDsTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = numSnapshots,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::ShaderResource,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        auto rgUpload = renderGraph.RegisterExternalResource( uploadBuffer.Get() );
        auto rgDrawCallIDs = renderGraph.CreateBuffer( drawCallIDsTrait, "Visibility.DrawCallIDs" );

        BEGIN_RG_RESOURCE_STRUCT( UploadPassResource )
            DECLARE_RG_BUFFER_COPY_SOURCE( upload )
            DECLARE_RG_BUFFER_COPY_DEST( drawCallIDs )
        END_RG_RESOURCE_STRUCT();

        UploadPassResource uploadPassResource = {
            .m_upload = rgUpload,
            .m_drawCallIDs = rgDrawCallIDs
        };

        renderGraph.AddPass(
            uploadPassResource,
            [uploadPassResource]( CopyCommandList& commandList )
            {
                commandList.CopyResource( uploadPassResource.m_drawCallIDs->Get(),
                                          uploadPassResource.m_upload->Get(),
                                          false );
            } );

        // Draw visibility buffer
        auto rgDepthStencil = renderGraph.RegisterExternalResource( param.m_depthStencil );

        BEGIN_RG_RESOURCE_STRUCT( VisibilityPassResource )
            DECLARE_RG_BUFFER_NONPIXEL_SRV( drawCallIDs )
        END_RG_RESOURCE_STRUCT();

        VisibilityPassResource passResource = {
            .m_drawCallIDs = rgDrawCallIDs,
        };

        RenderViewGroup& renderViewGroup = param.m_renderViewGroup;
        auto [width, height] = renderViewGroup.GetViewport().Size();

        RasterOutput rasterOutput;
        rasterOutput.SetRenderTarget( 0, param.m_visibility, RasterOutputLoadAction::Clear );
        rasterOutput.SetDepthStencil( rgDepthStencil, true );
        rasterOutput.SetViewport( width, height );
        rasterOutput.SetScissorRect( width, height );

        VertexBuffer primitiveIds = GetPrimitiveIdPool().Alloc( static_cast<uint32>( visibilityPassSnapshots.size() * sizeof( uint32 ) ) );
        UpdatePrimitiveIDs( visibilityPassSnapshots, primitiveIds );

        renderGraph.AddPass(
            passResource,
            rasterOutput,
            [passResource, resourceBinder, visibilityPassSnapshots, primitiveIds]( CommandList& commandList ) mutable
            {
                // Update invalidated resources
                for ( auto& viewDrawSnapshot : visibilityPassSnapshots )
                {
                    DrawSnapshot& snapshot = *viewDrawSnapshot.m_drawSnapshot;
                    GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

                    resourceBinder.Add( StaticName( "DrawCallIDs" ), passResource.m_drawCallIDs->SRV() );
                    resourceBinder.Bind( pipelineState.m_shaderState, snapshot.m_shaderBindings );
                }

                ParallelCommitDrawSnapshot( commandList, visibilityPassSnapshots, primitiveIds );
            } );
    }

    RefHandle<agl::Buffer> VisibilityBuffer::UploadPrimitiveIds( const VisibilityPassData& passData )
    {
        if ( passData.m_shadingSnapshots.empty() )
        {
            return {};
        }

        auto numElement = static_cast<uint32>( passData.m_shadingSnapshots.size() + 1 );

        agl::BufferTrait trait = {
            .m_stride = sizeof( uint32 ),
            .m_count = numElement,
            .m_access = agl::ResourceAccess::Upload,
            .m_bindType = agl::ResourceBindType::ShaderResource,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        RefHandle<agl::Buffer> buffer = agl::Buffer::Create( trait, "Visibility.PrimitiveIds" );
        buffer->Init();

        auto data = GraphicsInterface().Lock<uint32>( buffer.Get() ) + 1;
        for ( const auto& shadingSnapshot : passData.m_shadingSnapshots )
        {
            *data++ = shadingSnapshot.m_primitiveId;
        }
        GraphicsInterface().UnLock( buffer.Get() );

        return buffer;
    }

    VisibilityBuffer::CountDrawCallIdOutput VisibilityBuffer::CountDrawCallId( RenderGraph& renderGraph, const CountDrawCallIdParam& param )
    {
        if ( param.IsValid() == false )
        {
            return {};
        }

        uint32 numElement = param.m_numDrawCallIds;

        agl::BufferTrait counterTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = numElement,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        auto rgCounter = renderGraph.CreateBuffer( counterTrait, "Visibility.Counter" );

        agl::BufferTrait indirectArgsTrait = {
            .m_stride = sizeof( uint32 ) * 3,
            .m_count = numElement,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
            .m_miscFlag = agl::ResourceMisc::DrawIndirectArgs | agl::ResourceMisc::BufferAllowRawViews,
            .m_format = agl::ResourceFormat::R32_TYPELESS,
        };

        auto rgIndirectArgs = renderGraph.CreateBuffer( indirectArgsTrait, "Visibility.IndirectArgs" );

        BEGIN_RG_RESOURCE_STRUCT( InitCounterPassResource )
            DECLARE_RG_BUFFER_UAV( counter )
            DECLARE_RG_BUFFER_UAV( indirectArgs )
        END_RG_RESOURCE_STRUCT();

        InitCounterPassResource initCounterPassResource = {
            .m_counter = rgCounter,
            .m_indirectArgs = rgIndirectArgs,
        };

        {
            GPU_PROFILE_EVENT( renderGraph, InitCounter )

            renderGraph.AddPass(
                initCounterPassResource,
                [initCounterPassResource, numDrawCallIds = param.m_numDrawCallIds]( ComputeCommandList& commandList )
                {
                    RenderGraphBuffer& counter = *initCounterPassResource.m_counter;
                    RenderGraphBuffer& indirectArgs = *initCounterPassResource.m_indirectArgs;

                    InitCounterCS initCounterCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( initCounterCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( initCounterCS );
                    BindResource( shaderBindings, initCounterCS.Counter(), counter.Get() );
                    BindResource( shaderBindings, initCounterCS.IndirectArgs(), indirectArgs.Get() );

                    SetShaderValue( commandList, initCounterCS.NumDrawCallIds(), numDrawCallIds );

                    commandList.BindShaderResources( shaderBindings );

                    uint32 numThreadGroupX = CalcAlignment<uint32>( numDrawCallIds, 32 ) / 32;
                    commandList.Dispatch( numThreadGroupX, 1 );
                } );
        }

        BEGIN_RG_RESOURCE_STRUCT( CountDrawCallIdPassResource )
            DECLARE_RG_TEXTURE_NONPIXEL_SRV( visibility )
            DECLARE_RG_BUFFER_UAV( counter )
        END_RG_RESOURCE_STRUCT();

        CountDrawCallIdPassResource countDrawCallIdPassResource = {
            .m_visibility = param.m_visibility,
            .m_counter = rgCounter,
        };

        {
            GPU_PROFILE_EVENT( renderGraph, CountDrawCallId )

            renderGraph.AddPass(
                countDrawCallIdPassResource,
                [countDrawCallIdPassResource]( ComputeCommandList& commandList )
                {
                    RenderGraphTexture& visibility = *countDrawCallIdPassResource.m_visibility;
                    RenderGraphBuffer& counter = *countDrawCallIdPassResource.m_counter;

                    CountDrawCallIdCS countDrawCallIdCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( countDrawCallIdCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( countDrawCallIdCS );
                    BindResource( shaderBindings, countDrawCallIdCS.Visibility(), visibility.Get() );
                    BindResource( shaderBindings, countDrawCallIdCS.Counter(), counter.Get() );

                    uint32 width = visibility.GetTrait().m_width;
                    uint32 height = visibility.GetTrait().m_height;

                    uint32 screenSize[2] = {
                        width,
                        height
                    };
                    SetShaderValue( commandList, countDrawCallIdCS.ScreenSize(), screenSize );

                    commandList.BindShaderResources( shaderBindings );

                    constexpr uint32 numThreadX = 8;
                    constexpr uint32 numThreadY = 4;

                    uint32 numThreadGroupX = CalcAlignment( width, numThreadX ) / numThreadX;
                    uint32 numThreadGroupY = CalcAlignment( height, numThreadY ) / numThreadY;
                    commandList.Dispatch( numThreadGroupX, numThreadGroupY );
                } );
        }

        return CountDrawCallIdOutput{
            .m_counter = rgCounter,
            .m_indirectArgs = rgIndirectArgs,
        };
    }

    RenderGraphBuffer* VisibilityBuffer::CalcCounterPrefixSum( RenderGraph& renderGraph, const CalcConterPrefixSumParam& param )
    {
        if ( param.IsValid() == false )
        {
            return nullptr;
        }

        agl::BufferTrait blockIdTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = 1,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        agl::BufferTrait blockStatusTrait = {
            .m_stride = sizeof( uint32 ) * 2,
            .m_count = CalcAlignment( param.m_numDrawCallIds, PrefixSumBlockSize ) / PrefixSumBlockSize,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        auto rgBlockId = renderGraph.CreateBuffer( blockIdTrait, "PrefixSum.BlockId" );
        auto rgBlockStatus = renderGraph.CreateBuffer( blockStatusTrait, "PrefixSum.BlockStatus" );

        BEGIN_RG_RESOURCE_STRUCT( InitPrefixSumPassResource )
            DECLARE_RG_BUFFER_UAV( blockId )
            DECLARE_RG_BUFFER_UAV( blockStatus )
        END_RG_RESOURCE_STRUCT();

        InitPrefixSumPassResource initPrefixSumPassResource = {
            .m_blockId = rgBlockId,
            .m_blockStatus = rgBlockStatus
        };

        uint32 numElements = param.m_numDrawCallIds;

        {
            GPU_PROFILE_EVENT( renderGraph, InitPrefixSum )

            renderGraph.AddPass(
                initPrefixSumPassResource,
                [initPrefixSumPassResource, numElements]( ComputeCommandList& commandList )
                {
                    agl::Buffer* blockId = initPrefixSumPassResource.m_blockId->Get();
                    agl::Buffer* blockStatus = initPrefixSumPassResource.m_blockStatus->Get();

                    InitPrefixSumBufferCS initPrefixSumCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( initPrefixSumCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( initPrefixSumCS );
                    BindResource( shaderBindings, initPrefixSumCS.BlockId(), blockId );
                    BindResource( shaderBindings, initPrefixSumCS.BlockStatus(), blockStatus );

                    uint32 numBlocks = CalcAlignment( numElements, PrefixSumBlockSize ) / PrefixSumBlockSize;
                    SetShaderValue( commandList, initPrefixSumCS.NumBlocks(), numBlocks );

                    commandList.BindShaderResources( shaderBindings );

                    uint32 numThreadGroupX = CalcAlignment<uint32>( numBlocks, 32 ) / 32;
                    commandList.Dispatch( numThreadGroupX, 1 );
                } );
        }

        agl::BufferTrait offsetTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = param.m_numDrawCallIds,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        auto rgOffset = renderGraph.CreateBuffer( offsetTrait, "Visibility.Offset" );

        BEGIN_RG_RESOURCE_STRUCT( PrefixSumPassResource )
            DECLARE_RG_BUFFER_UAV( input )
            DECLARE_RG_BUFFER_UAV( blockId )
            DECLARE_RG_BUFFER_UAV( blockStatus )
            DECLARE_RG_BUFFER_UAV( output )
        END_RG_RESOURCE_STRUCT();

        PrefixSumPassResource prefixSumPassResource = {
            .m_input = param.m_counter,
            .m_blockId = rgBlockId,
            .m_blockStatus = rgBlockStatus,
            .m_output = rgOffset
        };

        {
            GPU_PROFILE_EVENT( renderGraph, PrefixSum )

            renderGraph.AddPass(
                prefixSumPassResource,
                [prefixSumPassResource, numElements]( ComputeCommandList& commandList )
                {
                    agl::Buffer* input = prefixSumPassResource.m_input->Get();
                    agl::Buffer* blockId = prefixSumPassResource.m_blockId->Get();
                    agl::Buffer* blockStatus = prefixSumPassResource.m_blockStatus->Get();
                    agl::Buffer* output = prefixSumPassResource.m_output->Get();

                    PrefixSumCS prefixSumCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( prefixSumCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( prefixSumCS );
                    BindResource( shaderBindings, prefixSumCS.Input(), input );
                    BindResource( shaderBindings, prefixSumCS.BlockId(), blockId );
                    BindResource( shaderBindings, prefixSumCS.BlockStatus(), blockStatus );
                    BindResource( shaderBindings, prefixSumCS.Output(), output );

                    SetShaderValue( commandList, prefixSumCS.NumItems(), numElements );

                    commandList.BindShaderResources( shaderBindings );

                    uint32 numBlocks = CalcAlignment( numElements, PrefixSumBlockSize ) / PrefixSumBlockSize;
                    commandList.Dispatch( numBlocks, 1 );
                } );
        }

        return rgOffset;
    }

    RenderGraphBuffer* VisibilityBuffer::BuildWorkList( RenderGraph& renderGraph, const BuildWorkListParam& param )
    {
        if ( param.IsValid() == false )
        {
            return nullptr;
        }

        uint32 width = param.m_visibility->GetTrait().m_width;
        uint32 height = param.m_visibility->GetTrait().m_height;

        agl::BufferTrait workListTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = width * height,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown,
        };

        auto rgWorkList = renderGraph.CreateBuffer( workListTrait, "Visibility.WorkList" );

        BEGIN_RG_RESOURCE_STRUCT( BuildWorkListPassResource )
            DECLARE_RG_TEXTURE_NONPIXEL_SRV( visibility )
            DECLARE_RG_BUFFER_UAV( offset )
            DECLARE_RG_BUFFER_UAV( indirectArgs )
            DECLARE_RG_BUFFER_UAV( workList )
        END_RG_RESOURCE_STRUCT();

        BuildWorkListPassResource buildWorkListPassResource = {
            .m_visibility = param.m_visibility,
            .m_offset = param.m_offset,
            .m_indirectArgs = param.m_indirectArgs,
            .m_workList = rgWorkList
        };

        {
            GPU_PROFILE_EVENT( renderGraph, BuildWorkList )

            renderGraph.AddPass(
                buildWorkListPassResource,
                [buildWorkListPassResource]( ComputeCommandList& commandList )
                {
                    RenderGraphTexture& visibility = *buildWorkListPassResource.m_visibility;
                    RenderGraphBuffer& offset = *buildWorkListPassResource.m_offset;
                    RenderGraphBuffer& indirectArgs = *buildWorkListPassResource.m_indirectArgs;
                    RenderGraphBuffer& workList = *buildWorkListPassResource.m_workList;

                    BuildWorkListCS buildWorkListCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( buildWorkListCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( buildWorkListCS );
                    BindResource( shaderBindings, buildWorkListCS.Visibility(), visibility.Get() );
                    BindResource( shaderBindings, buildWorkListCS.Offset(), offset.Get() );
                    BindResource( shaderBindings, buildWorkListCS.IndirectArgs(), indirectArgs.Get() );
                    BindResource( shaderBindings, buildWorkListCS.WorkList(), workList.Get() );

                    uint32 width = visibility.GetTrait().m_width;
                    uint32 height = visibility.GetTrait().m_height;

                    uint32 screenSize[2] = {
                        width,
                        height
                    };
                    SetShaderValue( commandList, buildWorkListCS.ScreenSize(), screenSize );

                    commandList.BindShaderResources( shaderBindings );

                    constexpr uint32 numThreadX = 8;
                    constexpr uint32 numThreadY = 4;

                    uint32 numThreadGroupX = CalcAlignment( width, numThreadX ) / numThreadX;
                    uint32 numThreadGroupY = CalcAlignment( height, numThreadY ) / numThreadY;
                    commandList.Dispatch( numThreadGroupX, numThreadGroupY );
                });
        }

        BEGIN_RG_RESOURCE_STRUCT( FinalizeIndirectArgsPassResource )
            DECLARE_RG_BUFFER_UAV( indirectArgs )
        END_RG_RESOURCE_STRUCT();

        FinalizeIndirectArgsPassResource finalizeIndirectArgsPassResource = {
            .m_indirectArgs = param.m_indirectArgs,
        };

        {
            GPU_PROFILE_EVENT( renderGraph, FinalizeIndirectArgs )

            renderGraph.AddPass(
                finalizeIndirectArgsPassResource,
                [finalizeIndirectArgsPassResource, numDrawCallIds = param.m_numDrawCallIds]( ComputeCommandList& commandList )
                {
                    RenderGraphBuffer& indirectArgs = *finalizeIndirectArgsPassResource.m_indirectArgs;

                    FinalizeIndirectArgsCS finalizeIndirectArgsCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( finalizeIndirectArgsCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( finalizeIndirectArgsCS );
                    BindResource( shaderBindings, finalizeIndirectArgsCS.IndirectArgs(), indirectArgs.Get() );

                    SetShaderValue( commandList, finalizeIndirectArgsCS.NumDrawCallIds(), numDrawCallIds );

                    commandList.BindShaderResources( shaderBindings );

                    constexpr uint32 numThreadX = 32;

                    uint32 numThreadGroupX = CalcAlignment( numDrawCallIds, numThreadX ) / numThreadX;
                    commandList.Dispatch( numThreadGroupX, 1 );
                } );
        }

        return rgWorkList;
    }

    void VisibilityBuffer::PrefixSumTestBed( RenderGraph& renderGraph )
    {
        GPU_PROFILE_EVENT( renderGraph, PrefixSumTestBed )

        constexpr uint32 numElements = 10000;

        // input
        agl::BufferTrait inputUploaderTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = numElements,
            .m_access = agl::ResourceAccess::Upload,
            .m_bindType = agl::ResourceBindType::None,
            .m_miscFlag = agl::ResourceMisc::None,
            .m_format = agl::ResourceFormat::Unknown
        };

        RefHandle<agl::Buffer> inputUploader = GraphicsResourcePool::GetInstance().FindFreeBuffer( inputUploaderTrait, "PrefixTestBed.InputUploader" );
        if ( uint32* dest = GraphicsInterface().Lock<uint32>( inputUploader.Get() ) )
        {
            std::fill_n( dest, numElements, 1 );
            GraphicsInterface().UnLock( inputUploader.Get() );
        }

        auto rgInputUploader = renderGraph.RegisterExternalResource( inputUploader.Get() );

        agl::BufferTrait inputTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = numElements,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        auto rgInput = renderGraph.CreateBuffer( inputTrait, "PrefixSumTestBed.Input" );

        BEGIN_RG_RESOURCE_STRUCT( UploadInputPassResource )
            DECLARE_RG_BUFFER_COPY_SOURCE( inputUploader )
            DECLARE_RG_BUFFER_COPY_DEST( input )
        END_RG_RESOURCE_STRUCT();

        UploadInputPassResource uploadInputPassResource = {
            .m_inputUploader = rgInputUploader,
            .m_input = rgInput
        };

        {
            GPU_PROFILE_EVENT( renderGraph, UploadInput )

            renderGraph.AddPass(
                uploadInputPassResource,
                [uploadInputPassResource]( CopyCommandList& commandList )
                {
                    commandList.CopyResource( uploadInputPassResource.m_input->Get(), uploadInputPassResource.m_inputUploader->Get(), false );
                } );
        }

        // prepare to prefix sum
        agl::BufferTrait blockIdTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = 1,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        agl::BufferTrait blockStatusTrait = {
            .m_stride = sizeof( uint32 ) * 2,
            .m_count = CalcAlignment( numElements, PrefixSumBlockSize ) / PrefixSumBlockSize,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        auto rgBlockId = renderGraph.CreateBuffer( blockIdTrait, "PrefixTestBed.BlockId" );
        auto rgBlockStatus = renderGraph.CreateBuffer( blockStatusTrait, "PrefixTestBed.BlockStatus" );

        BEGIN_RG_RESOURCE_STRUCT( InitPrefixSumPassResource )
            DECLARE_RG_BUFFER_UAV( blockId )
            DECLARE_RG_BUFFER_UAV( blockStatus )
        END_RG_RESOURCE_STRUCT();

        InitPrefixSumPassResource initPrefixSumPassResource = {
            .m_blockId = rgBlockId,
            .m_blockStatus = rgBlockStatus
        };

        {
            GPU_PROFILE_EVENT( renderGraph, InitPrefixSum )

            renderGraph.AddPass(
                initPrefixSumPassResource,
                [initPrefixSumPassResource, numElements]( ComputeCommandList& commandList )
                {
                    agl::Buffer* blockId = initPrefixSumPassResource.m_blockId->Get();
                    agl::Buffer* blockStatus = initPrefixSumPassResource.m_blockStatus->Get();

                    InitPrefixSumBufferCS initPrefixSumCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( initPrefixSumCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( initPrefixSumCS );
                    BindResource( shaderBindings, initPrefixSumCS.BlockId(), blockId );
                    BindResource( shaderBindings, initPrefixSumCS.BlockStatus(), blockStatus );

                    uint32 numBlocks = CalcAlignment( numElements, PrefixSumBlockSize ) / PrefixSumBlockSize;
                    SetShaderValue( commandList, initPrefixSumCS.NumBlocks(), numBlocks );

                    commandList.BindShaderResources( shaderBindings );

                    uint32 numThreadGroupX = CalcAlignment<uint32>( numBlocks, 32 ) / 32;
                    commandList.Dispatch( numThreadGroupX, 1 );
                } );
        }

        // output
        agl::BufferTrait outputTrait = {
            .m_stride = sizeof( uint32 ),
            .m_count = numElements,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown
        };

        auto rgOutput = renderGraph.CreateBuffer( outputTrait, "PrefixTestBed.Output" );

        BEGIN_RG_RESOURCE_STRUCT( PrefixSumPassResource )
            DECLARE_RG_BUFFER_UAV( input )
            DECLARE_RG_BUFFER_UAV( blockId )
            DECLARE_RG_BUFFER_UAV( blockStatus )
            DECLARE_RG_BUFFER_UAV( output )
        END_RG_RESOURCE_STRUCT();

        PrefixSumPassResource prefixSumPassResource = {
            .m_input = rgInput,
            .m_blockId = rgBlockId,
            .m_blockStatus = rgBlockStatus,
            .m_output = rgOutput
        };

        {
            GPU_PROFILE_EVENT( renderGraph, PrefixSum )

            renderGraph.AddPass(
                prefixSumPassResource,
                [prefixSumPassResource, numElements]( ComputeCommandList& commandList )
                {
                    agl::Buffer* input = prefixSumPassResource.m_input->Get();
                    agl::Buffer* blockId = prefixSumPassResource.m_blockId->Get();
                    agl::Buffer* blockStatus = prefixSumPassResource.m_blockStatus->Get();
                    agl::Buffer* output = prefixSumPassResource.m_output->Get();

                    PrefixSumCS prefixSumCS;
                    RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( prefixSumCS );

                    commandList.BindPipelineState( pso.Get() );

                    agl::ShaderBindings shaderBindings = CreateShaderBindings( prefixSumCS );
                    BindResource( shaderBindings, prefixSumCS.Input(), input );
                    BindResource( shaderBindings, prefixSumCS.BlockId(), blockId );
                    BindResource( shaderBindings, prefixSumCS.BlockStatus(), blockStatus );
                    BindResource( shaderBindings, prefixSumCS.Output(), output );

                    SetShaderValue( commandList, prefixSumCS.NumItems(), numElements );

                    commandList.BindShaderResources( shaderBindings );

                    uint32 numBlocks = CalcAlignment( numElements, PrefixSumBlockSize ) / PrefixSumBlockSize;
                    commandList.Dispatch( numBlocks, 1 );
                } );
        }
    }
}
