#include "TextureStreaming.h"

#include "Config/DefaultAglConfig.h"
#include "ConsoleMessage/ConVar.h"
#include "MaterialResource.h"

#include <atomic>

namespace
{
    engine::ConVariable CVarTextureStreamingEnabled(
        "r.TextureStreaming.Enabled",
        "0",
        "Enables dynamic texture mip streaming based on runtime visibility." );

    engine::ConVariable CVarTextureStreamingMipBias(
        "r.TextureStreaming.MipBias",
        "0",
        "Controls texture streaming mip bias. "
        "Positive values favor lower-resolution mip levels to reduce memory usage, "
        "while negative values favor higher-resolution mip levels." );
}

namespace rendercore
{
    TextureStreamingManager& TextureStreamingManager::GetInstance()
    {
        assert( IsInGameThread() );
        static TextureStreamingManager instance;
        return instance;
    }

    void TextureStreamingManager::BootUp()
    {
        // +1 to account for GPU readback latency and avoid reusing a feedback buffer that may still be in flight on the GPU.
        m_feedbacks.resize( agl::DefaultAgl::GetBufferCount() + 1 );

        m_pendingFeedbackIndices.reserve( m_feedbacks.size() );
        m_freeFeedbackIndices.reserve( m_feedbacks.size() );

        for ( int32 i = 0; i < m_feedbacks.size(); ++i )
        {
            m_freeFeedbackIndices.push_back( i );
        }

        EnqueueRenderTask(
            []()
            {
                renderthread::TextureStreamingManager::GetInstance().BootUp();
            });
    }

    void TextureStreamingManager::Shutdown()
    {
        for ( size_t i = 0; i < m_materials.GetMaxIndex(); ++i )
        {
            Unregister( static_cast<int32>( i ) );
        }
    }

    void TextureStreamingManager::Register( Material& material )
    {
        auto materialId = static_cast<int32>( m_materials.Add( &material ) );
        material.m_materialId = materialId;
    }

    void TextureStreamingManager::Unregister( int32 id )
    {
        if ( m_materials.IsAllocated( id ) )
        {
            m_materials.RemoveAt( id );
        }
    }

    void TextureStreamingManager::Tick()
    {
        int32 latestIndex = -1;
        uint64 latestFrameIndex = 0;

        for ( auto iter = std::begin( m_pendingFeedbackIndices ); iter != std::end( m_pendingFeedbackIndices ); )
        {
            TextureStreamingFeedback& feedback = m_feedbacks[*iter];

            if ( std::atomic_ref<bool>( feedback.m_hasNewData ).load( std::memory_order_acquire ) )
            {
                m_freeFeedbackIndices.push_back( *iter );
                if ( feedback.m_frameIndex > latestFrameIndex )
                {
                    latestIndex = *iter;
                    latestFrameIndex = feedback.m_frameIndex;
                }

                iter = m_pendingFeedbackIndices.erase( iter );
                continue;
            }

            ++iter;
        }

        if ( latestIndex > -1 )
        {
            TextureStreamingFeedback& feedback = m_feedbacks[latestIndex];

            using TextureMinimumSizes = std::map<Texture*, int32, std::less<Texture*>, TransientAllocator<std::pair<Texture* const, int32>, ThreadType::GameThread>>;
            TextureMinimumSizes textureMinimumSizes;

            for ( auto& material : m_materials )
            {
                int32 materialId = material->GetMaterialId();
                if ( feedback.m_minTextureSizes.size() <= materialId )
                {
                    continue;
                }

                int32 minSize = feedback.m_minTextureSizes[materialId];
                for ( const auto& property : material->GetProperties() | std::views::values )
                {
                    auto textureProperty = Cast<const TextureProperty>( property.get() );
                    if ( textureProperty == nullptr )
                    {
                        continue;
                    }

                    Texture* texture = textureProperty->Value().get();
                    if ( texture == nullptr )
                    {
                        continue;
                    }

                    if ( auto found = textureMinimumSizes.find( texture ); found != std::end( textureMinimumSizes ) )
                    {
                        found->second = std::max( found->second, minSize );
                    }
                    else
                    {
                        textureMinimumSizes[texture] = minSize;
                    }
                }
            }

            int32 mipBias = CVarTextureStreamingMipBias.GetInteger();

            auto CalcMipLevelForMinSize = [mipBias]( const Texture& texture, int32 minSize )
            {
                const int32 smallestDimension = std::min( texture.GetWidth(), texture.GetHeight() );
                const int32 safeMinSize = std::min( std::max( minSize, 1 ), smallestDimension );
                return static_cast<int32>( std::floor( std::log2( smallestDimension / safeMinSize ) ) ) + mipBias;
            };

            for ( auto& [texture, minSize] : textureMinimumSizes )
            {
                int32 desiredMip = CalcMipLevelForMinSize( *texture, minSize );
                texture->RequestMipLevels( desiredMip );
            }
        }

        RequestStreamingFeedback();

        ++m_frameIndex;
    }

    void TextureStreamingManager::RequestStreamingFeedback()
    {
        if ( ( m_materials.Size() == 0 ) || m_freeFeedbackIndices.empty() )
        {
            return;
        }

        int32 freeFeedbackIndex = m_freeFeedbackIndices.back();
        m_freeFeedbackIndices.pop_back();

        m_pendingFeedbackIndices.push_back( freeFeedbackIndex );

        TextureStreamingFeedback& feedback = m_feedbacks[freeFeedbackIndex];

        feedback.m_minTextureSizes.resize( m_materials.GetMaxIndex() );
        feedback.m_frameIndex = m_frameIndex;

        auto hasNewData = std::atomic_ref<bool>( feedback.m_hasNewData );
        hasNewData.store( false, std::memory_order_relaxed );

        EnqueueRenderTask( [&feedback]()
        {
            renderthread::TextureStreamingManager::GetInstance().EnqueueFeedback( feedback );
        } );
    }
}

namespace rendercore::renderthread
{
    class ClearTextureFeedbackCS final : public GlobalShaderBase<ComputeShader, ClearTextureFeedbackCS>
    {
        DECLARE_SHADER_PARAM( NumElement );
        DECLARE_SHADER_PARAM( TextureFeedback );
    };

    REGISTER_GLOBAL_SHADER( ClearTextureFeedbackCS, "Common/CS_ClearTextureFeedback.fx", "main" );

    TextureStreamingManager& TextureStreamingManager::GetInstance()
    {
        assert( IsInRenderThread() );
        static TextureStreamingManager instance;
        return instance;
    }

    void TextureStreamingManager::BootUp()
    {
        // Reserve the same size as the game thread TextureStreamingManager
        m_feedbackQueue.reserve( agl::DefaultAgl::GetBufferCount() + 1 );
    }

    void TextureStreamingManager::Shutdown()
    {
        m_feedbackQueue.clear();
    }

    void TextureStreamingManager::EnqueueFeedback( TextureStreamingFeedback& feedback )
    {
        TextureStreamingFeedbackContext context = {
            .m_feedback = &feedback,
            .m_frameIndex = m_frameIndex,
        };

        m_feedbackQueue.push_back( context );
    }

    agl::Buffer* TextureStreamingManager::AddFeedbackBufferClearPass( RenderGraph& renderGraph )
    {
        TextureStreamingFeedbackContext* context = GetCurrentFeedbackContext();

        uint32 numElement = context ? static_cast<uint32>( context->m_feedback->m_minTextureSizes.size() ) : 1;

        agl::BufferDesc gpuFeedbackDesc = {
            .m_stride = sizeof( int32 ),
            .m_count = numElement,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::BufferStructured,
            .m_format = agl::ResourceFormat::Unknown,
        };

        auto rgGpuFeedBack = renderGraph.CreateBuffer( gpuFeedbackDesc, "TextureStreaming.GpuFeedback" );

        BEGIN_RG_RESOURCE_STRUCT( ClearFeedbackBufferPassResource )
            DECLARE_RG_BUFFER_UAV( gpuFeedback )
        END_RG_RESOURCE_STRUCT();

        ClearFeedbackBufferPassResource passResource = {
            .m_gpuFeedback = rgGpuFeedBack
        };

        renderGraph.AddPass(
            passResource,
            [passResource, numElement]( ComputeCommandList& commandList )
            {
                ClearTextureFeedbackCS clearTextureFeedbackCS;

                RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( clearTextureFeedbackCS );

                commandList.BindPipelineState( pso.Get() );

                agl::ShaderBindings shaderBindings = CreateShaderBindings( clearTextureFeedbackCS );
                BindResource( shaderBindings, clearTextureFeedbackCS.TextureFeedback(), passResource.m_gpuFeedback->Get() );

                SetShaderValue( commandList, clearTextureFeedbackCS.NumElement(), numElement );

                commandList.BindShaderResources( shaderBindings );

                uint32 numThreadGroupX = CalcAlignment<uint32>( numElement, 32 ) / 32;
                commandList.Dispatch( numThreadGroupX, 1 );
            } );

        return renderGraph.ConvertToExternalResource( rgGpuFeedBack );
    }

    void TextureStreamingManager::AddFeedbackBufferReadbackPass( RenderGraph& renderGraph, agl::Buffer* gpuFeedback )
    {
        TextureStreamingFeedbackContext* context = GetCurrentFeedbackContext();
        if ( context == nullptr )
        {
            return;
        }

        agl::BufferDesc readbackDesc = gpuFeedback->GetDesc();
        readbackDesc.m_access = agl::ResourceAccess::Readback;
        readbackDesc.m_bindType = agl::ResourceBindType::None;
        readbackDesc.m_miscFlag = agl::ResourceMisc::None;
        readbackDesc.m_format = agl::ResourceFormat::Unknown;

        auto rgFeedback = renderGraph.RegisterExternalResource( gpuFeedback );
        auto rgReadback = renderGraph.CreateBuffer( readbackDesc, "TextureStreaming.Readback" );

        BEGIN_RG_RESOURCE_STRUCT( FeedbackBufferReadbackPassResource )
            DECLARE_RG_BUFFER_COPY_SOURCE( feedback )
            DECLARE_RG_BUFFER_COPY_DEST( readback )
        END_RG_RESOURCE_STRUCT();

        FeedbackBufferReadbackPassResource passResource = {
            .m_feedback = rgFeedback,
            .m_readback = rgReadback
        };

        renderGraph.AddPass(
            passResource,
            [passResource]( CopyCommandList& commandList )
            {
                auto readback = passResource.m_readback->Get();
                auto feedback = passResource.m_feedback->Get();
                commandList.CopyResource( readback, feedback, false );
            } );

        context->m_gpuFeedback = renderGraph.ConvertToExternalResource( rgReadback );
    }

    void TextureStreamingManager::Tick()
    {
        uint64 nextFrameIndex = m_frameIndex + 1;
        uint64 bufferCount = agl::DefaultAgl::GetBufferCount();

        bool textureStreamingEnabled = CVarTextureStreamingEnabled.GetBool();

        for ( auto iter = std::begin( m_feedbackQueue ); iter != std::end( m_feedbackQueue ); )
        {
            TextureStreamingFeedback& feedback = *iter->m_feedback;

            if ( textureStreamingEnabled )
            {
                if ( ( iter->m_frameIndex % bufferCount ) != ( nextFrameIndex % bufferCount ) )
                {
                    ++iter;
                    continue;
                }

                auto begin = GraphicsInterface().Lock<int32>( iter->m_gpuFeedback.Get(), agl::ResourceLockFlag::Read );
                auto end = begin + feedback.m_minTextureSizes.size();
                std::copy( begin, end, std::begin( feedback.m_minTextureSizes ) );
                GraphicsInterface().UnLock( iter->m_gpuFeedback.Get() );
            }
            else
            {
                std::ranges::fill( feedback.m_minTextureSizes, std::numeric_limits<int32>::max() );
            }

            std::atomic_ref<bool>( feedback.m_hasNewData ).store( true, std::memory_order_release );

            iter = m_feedbackQueue.erase( iter );
        }

        m_frameIndex = nextFrameIndex;
    }

    TextureStreamingFeedbackContext* TextureStreamingManager::GetCurrentFeedbackContext()
    {
        auto found = std::ranges::find_if( m_feedbackQueue,
                              [this]( const TextureStreamingFeedbackContext& context )
                              {
                                  return context.m_frameIndex == m_frameIndex;
                              } );

        if ( found == std::end( m_feedbackQueue ) )
        {
            return nullptr;
        }

        return &*found;
    }
}
