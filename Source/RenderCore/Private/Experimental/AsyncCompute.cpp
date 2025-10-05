#include "Experimental/AsyncCompute.h"

#include "GlobalShaders.h"
#include "RenderGraph.h"

namespace rendercore
{
    class HeavyWorkCS final : public GlobalShaderCommon<ComputeShader, HeavyWorkCS>
    {
        DEFINE_SHADER_PARAM( Output );
    };

    REGISTER_GLOBAL_SHADER( HeavyWorkCS, "./Assets/Shaders/Experimental/AsyncCompute/CS_HeavyWork.asset" );

    void AsyncComputeTestBed::IndependentHeavyWork( RenderGraph& renderGraph )
    {
        agl::TextureTrait outputTrait = {
            .m_width = 2048,
            .m_height = 2048,
            .m_depth = 1,
            .m_sampleCount = 1,
            .m_sampleQuality = 0,
            .m_mipLevels = 1,
            .m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
            .m_access = agl::ResourceAccessFlag::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::None
        };

        auto rgOutput = renderGraph.CreateTexture( outputTrait, "Output" );

        BEGIN_RG_RESOURCE_STRUCT( IndependentHeavyWorkResource )
            DECLARE_RG_TEXTURE_UAV( output )
        END_RG_RESOURCE_STRUCT();

        IndependentHeavyWorkResource independentHeavyWorkResource = {
            .m_output = rgOutput,
        };

        renderGraph.AddPass(
            true,
            independentHeavyWorkResource,
            [independentHeavyWorkResource]( ComputeCommandList& commandList )
            {
                HeavyWorkCS heavyWorkCS;
                RefHandle<agl::ComputePipelineState> heavyWorkPSO = PrepareComputePipelineState( heavyWorkCS );

                commandList.BindPipelineState( heavyWorkPSO.Get() );

                agl::ShaderBindings shaderBindings = CreateShaderBindings( heavyWorkCS );
                BindResource( shaderBindings, heavyWorkCS.Output(), independentHeavyWorkResource.m_output->Get() );

                commandList.BindShaderResources( shaderBindings );

                commandList.Dispatch( 2048 / 8, 2048 / 8, 1 );
            } );
    }

    void AsyncComputeTestBed::DependentHeavyWork( RenderGraph& renderGraph )
    {
        agl::TextureTrait outputTrait = {
            .m_width = 2048,
            .m_height = 2048,
            .m_depth = 1,
            .m_sampleCount = 1,
            .m_sampleQuality = 0,
            .m_mipLevels = 1,
            .m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
            .m_access = agl::ResourceAccessFlag::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
            .m_miscFlag = agl::ResourceMisc::None
        };

        auto rgOutput = renderGraph.CreateTexture( outputTrait, "Output" );

        BEGIN_RG_RESOURCE_STRUCT( PrerequisiteWorkResource )
            DECLARE_RG_TEXTURE_UAV( output )
        END_RG_RESOURCE_STRUCT();

        PrerequisiteWorkResource prerequisiteWorkResource = {
            .m_output = rgOutput,
        };

        renderGraph.AddPass(
            true,
            prerequisiteWorkResource,
            [prerequisiteWorkResource]( ComputeCommandList& commandList )
            {
                HeavyWorkCS heavyWorkCS;
                RefHandle<agl::ComputePipelineState> heavyWorkPSO = PrepareComputePipelineState( heavyWorkCS );

                commandList.BindPipelineState( heavyWorkPSO.Get() );

                agl::ShaderBindings shaderBindings = CreateShaderBindings( heavyWorkCS );
                BindResource( shaderBindings, heavyWorkCS.Output(), prerequisiteWorkResource.m_output->Get() );

                commandList.BindShaderResources( shaderBindings );

                commandList.Dispatch( 2048 / 8, 2048 / 8, 1 );
            } );

        BEGIN_RG_RESOURCE_STRUCT( SubsequentWorkResource )
            DECLARE_RG_TEXTURE_NONPIXEL_SRV( input )
        END_RG_RESOURCE_STRUCT();

        SubsequentWorkResource subsequentWorkResource = {
            .m_input = rgOutput,
        };

        renderGraph.AddPass(
            false,
            subsequentWorkResource,
            [subsequentWorkResource]( ComputeCommandList& commandList )
            {
                HeavyWorkCS heavyWorkCS;
                RefHandle<agl::ComputePipelineState> heavyWorkPSO = PrepareComputePipelineState( heavyWorkCS );

                commandList.BindPipelineState( heavyWorkPSO.Get() );

                agl::ShaderBindings shaderBindings = CreateShaderBindings( heavyWorkCS );
                BindResource( shaderBindings, heavyWorkCS.Output(), subsequentWorkResource.m_input->Get() );

                commandList.BindShaderResources( shaderBindings );

                commandList.Dispatch( 2048 / 8, 2048 / 8, 1 );
            } );
    }
}
