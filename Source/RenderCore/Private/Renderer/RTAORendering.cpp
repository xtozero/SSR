#include "RTAORendering.h"

#include "GlobalShaders.h"
#include "RaytracingPipelineState.h"
#include "RaytracingShaderTable.h"
#include "RenderGraph.h"
#include "Scene/IScene.h"
#include "Scene/RaytracingScene.h"

namespace rendercore
{
    class RTAORayGen : public GlobalShaderBase<RayGenerationShader, RTAORayGen>
    {
        DEFINE_SHADER_PARAM( AccelerationStructure );
        DEFINE_SHADER_PARAM( SceneColor );
    };

    class RTAOClosestHit : public GlobalShaderBase<ClosestHitShader, RTAOClosestHit>
    {
    };

    class RTAOMiss : public GlobalShaderBase<MissShader, RTAOMiss>
    {
    };

    REGISTER_GLOBAL_SHADER( RTAORayGen, "RayTracing/RTAO.fx", "RayGen" );
    REGISTER_GLOBAL_SHADER( RTAOClosestHit, "RayTracing/RTAO.fx", "ClosestHit" );
    REGISTER_GLOBAL_SHADER( RTAOMiss, "RayTracing/RTAO.fx", "Miss" );

    void RTAO::ShaderTest( RenderGraph& renderGraph, RenderViewGroup& renderViewGroup )
    {
        if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() == false )
        {
            return;
        }

        RaytracingScene* raytracingScene = renderViewGroup.Scene().GetRaytracingScene();
        if ( ( raytracingScene == nullptr ) || ( raytracingScene->GetTLAS() == nullptr ) )
        {
            return;
        }

        auto [width, height] = renderViewGroup.GetViewport().Size();

        agl::TextureTrait trait = {
            .m_width = width,
            .m_height = height,
            .m_depth = 1,
            .m_sampleCount = 1,
            .m_sampleQuality = 0,
            .m_mipLevels = 1,
            .m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::None
        };

        auto rgAccelerationStructure = renderGraph.RegisterExternalResource( raytracingScene->GetTLAS()->Resource() );
        auto rgAO = renderGraph.CreateTexture( trait, "Test" );

        BEGIN_RG_RESOURCE_STRUCT( RTAOPassResource )
            DECLARE_RG_BUFFER_ACCELERATION_STRUCTURE( accelerationStructure )
            DECLARE_RG_TEXTURE_UAV( sceneColor )
        END_RG_RESOURCE_STRUCT();

        RTAOPassResource passResource = {
            .m_accelerationStructure = rgAccelerationStructure,
            .m_sceneColor = rgAO,
        };

        renderGraph.AddPass(
            passResource,
            [passResource]( ComputeCommandList& commandList )
            {
                RTAORayGen rtaoRayGen;
                RTAOClosestHit rtaoClosestHit;
                RTAOMiss rtaoMiss;

                agl::HitGroupDesc rtaoHitGroups[] = {
                    // HitGroup 0
                    {
                        .m_closestHit = rtaoClosestHit.Resource(),
                    }
                };

                agl::MissShader* rtaoMissShaders[] = {
                    // Miss 0
                    rtaoMiss.Resource()
                };

                agl::RaytracingPipelineStateDesc pipelineStateDesc = {
                    .m_shaderTableDesc = {
                        .m_rayGeneration = rtaoRayGen.Resource(),
                        .m_hitGroupDescs = rtaoHitGroups,
                        .m_misses = rtaoMissShaders,
                    },
                    .m_maxPayloadSizeInBytes = 4,
                    .m_maxAttributeSizeInBytes = sizeof( float2 ),
                    .m_maxTraceRecursionDepth = 1,
                };

                auto pipelineState = agl::RaytracingPipelineState::Create( pipelineStateDesc );
                agl::ShaderBindings shaderBindings = CreateShaderBindings( pipelineState.Get() );

                BindResource( shaderBindings, rtaoRayGen.AccelerationStructure(), passResource.m_accelerationStructure->Get() );
                BindResource( shaderBindings, rtaoRayGen.SceneColor(), passResource.m_sceneColor->Get() );

                uint32 width = passResource.m_sceneColor->GetTrait().m_width;
                uint32 height = passResource.m_sceneColor->GetTrait().m_height;

                commandList.DispatchRays( pipelineState.Get(), shaderBindings, width, height );
            } );
    }
}
