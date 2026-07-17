#include "RTAORendering.h"

#include "ConsoleMessage/ConVar.h"

#include "GlobalShaders.h"

#include "RaytracingPipelineState.h"
#include "RaytracingShaderTable.h"
#include "RenderGraph.h"

#include "Scene/IScene.h"

namespace
{
    engine::ConVariable CVarRTAOEnabled(
        "r.RTAO.Enabled",
        "1",
        "Enables or disables Ray Traced Ambient Occlusion (RTAO).");

    engine::ConVariable CVarRTAOIntensity(
        "r.RTAO.Intensity",
        "1",
        "Controls the intensity of Ray-Traced Ambient Occlusion (RTAO). "
        "Higher values result in stronger occlusion (darker contact shadows), "
        "while lower values produce a more subtle effect." );

    engine::ConVariable CVarRTAORadius(
        "r.RTAO.Radius",
        "100",
        "Defines the maximum ray distance for RTAO. "
        "Larger values result in wider, softer occlusion effects, but may impact performance." );

    engine::ConVariable CVarRTAOSampleCount(
        "r.RTAO.SampleCount",
        "8",
        "Controls the number of samples used for RTAO. "
        "Higher values improve quality and reduce noise, but increase cost." );

    engine::ConVariable CVarRTAODenoiseKernelRadius(
        "r.RTAO.Denoise.KernelRadius",
        "8",
        "Specifies the radius of the denoising kernel for RTAO. "
        "Larger values use more samples, reducing noise but increasing performance cost." );
}

namespace rendercore
{
    class RTAORayGen : public GlobalShaderBase<RayGenerationShader, RTAORayGen>
    {
        DECLARE_SHADER_PARAM( SceneViewParameters );

        DECLARE_SHADER_PARAM( AccelerationStructure );

        DECLARE_SHADER_PARAM( ViewSpaceDistance );
        DECLARE_SHADER_PARAM( WorldNormal );

        DECLARE_SHADER_PARAM( BlackBorderSampler );

        DECLARE_SHADER_PARAM( AmbientOcclusion );

        DECLARE_SHADER_PARAM( SampleCount );
        DECLARE_SHADER_PARAM( AORadius );
        DECLARE_SHADER_PARAM( AOIntensity );
    };

    class RTAOClosestHit : public GlobalShaderBase<ClosestHitShader, RTAOClosestHit>
    {
    };

    class RTAOMiss : public GlobalShaderBase<MissShader, RTAOMiss>
    {
    };

    REGISTER_GLOBAL_SHADER( RTAORayGen, "Raytracing/RTAO.fx", "RayGen" );
    REGISTER_GLOBAL_SHADER( RTAOClosestHit, "Raytracing/RTAO.fx", "ClosestHit" );
    REGISTER_GLOBAL_SHADER( RTAOMiss, "Raytracing/RTAO.fx", "Miss" );

    RefHandle<agl::Texture> RTAORenderPass::Render( RenderGraph& renderGraph, const RTAORenderParams& params )
    {
        if ( ( CVarRTAOEnabled.GetBool() == false )
            || ( GetInterface<agl::IAgl>()->SupportsHardwareRaytracing() == false )
            || ( params.IsValid() == false ) )
        {
            return WhiteTexture;
        }

        GPU_PROFILE_EVENT( renderGraph, RTAO );

        // Render RTAO
        agl::TextureDesc desc = {
            .m_width = params.m_screenWidth,
            .m_height = params.m_screenHeight,
            .m_depth = 1,
            .m_sampleCount = 1,
            .m_sampleQuality = 0,
            .m_mipLevels = 1,
            .m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
            .m_access = agl::ResourceAccess::Default,
            .m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
            .m_miscFlag = agl::ResourceMisc::None
        };

        auto rgAccelerationStructure = renderGraph.RegisterExternalResource( params.m_raytracingScene->Resource() );
        auto rgViewSpaceDistance = renderGraph.RegisterExternalResource( params.m_viewSpaceDistance.Get() );
        auto rgWorldNormal = renderGraph.RegisterExternalResource( params.m_worldNormal.Get() );
        auto rgAmbientOcclusion = renderGraph.CreateTexture( desc, "RTAO.AmbientOcclusion" );

        BEGIN_RG_RESOURCE_STRUCT( RTAOPassResource )
            DECLARE_RG_BUFFER_ACCELERATION_STRUCTURE( accelerationStructure )
            DECLARE_RG_TEXTURE_NONPIXEL_SRV( viewSpaceDistance )
            DECLARE_RG_TEXTURE_NONPIXEL_SRV( worldNormal )
            DECLARE_RG_TEXTURE_UAV( ambientOcclusion )
        END_RG_RESOURCE_STRUCT();

        RTAOPassResource passResource = {
            .m_accelerationStructure = rgAccelerationStructure,
            .m_viewSpaceDistance = rgViewSpaceDistance,
            .m_worldNormal = rgWorldNormal,
            .m_ambientOcclusion = rgAmbientOcclusion,
        };

        struct PassParams
        {
            RefHandle<agl::Buffer> m_viewShaderArguments;
            uint32 m_sampleCount;
            uint32 m_screenWidth;
            uint32 m_screenHeight;
            float m_aoRadius;
            float m_aoIntensity;
        } passParams = {
            .m_viewShaderArguments = params.m_viewShaderArguments,
            .m_sampleCount = static_cast<uint32>( CVarRTAOSampleCount.GetInteger() ),
            .m_screenWidth = params.m_screenWidth,
            .m_screenHeight = params.m_screenHeight,
            .m_aoRadius = CVarRTAORadius.GetFloat(),
            .m_aoIntensity = CVarRTAOIntensity.GetFloat(),
        };

        renderGraph.AddPass(
            passResource,
            [passResource, passParams]( ComputeCommandList& commandList )
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

                auto blackBorderSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
                    , agl::TextureAddressMode::Border
                    , agl::TextureAddressMode::Border
                    , agl::TextureAddressMode::Border
                    , 0.f
                    , agl::ComparisonFunc::Never
                    , Color( 0, 0, 0, 255 )>::Get();

                auto pipelineState = agl::RaytracingPipelineState::Create( pipelineStateDesc );
                agl::ShaderBindings shaderBindings = CreateShaderBindings( pipelineState.Get() );

                BindResource( shaderBindings, rtaoRayGen.SceneViewParameters(), passParams.m_viewShaderArguments.Get() );

                BindResource( shaderBindings, rtaoRayGen.AccelerationStructure(), passResource.m_accelerationStructure->Get() );
                BindResource( shaderBindings, rtaoRayGen.ViewSpaceDistance(), passResource.m_viewSpaceDistance->Get() );
                BindResource( shaderBindings, rtaoRayGen.WorldNormal(), passResource.m_worldNormal->Get() );

                BindResource( shaderBindings, rtaoRayGen.BlackBorderSampler(), blackBorderSampler );

                BindResource( shaderBindings, rtaoRayGen.AmbientOcclusion(), passResource.m_ambientOcclusion->Get() );

                SetShaderValue( commandList, rtaoRayGen.SampleCount(), passParams.m_sampleCount );
                SetShaderValue( commandList, rtaoRayGen.AORadius(), passParams.m_aoRadius );
                SetShaderValue( commandList, rtaoRayGen.AOIntensity(), passParams.m_aoIntensity );

                commandList.DispatchRays( pipelineState.Get(), shaderBindings, passParams.m_screenWidth, passParams.m_screenHeight );
            } );

        // Denoise
        auto rgPrevViewSpaceDistance = renderGraph.RegisterExternalResource( params.m_prevViewSpaceDistance.Get() );
        auto rgVelocity = renderGraph.RegisterExternalResource( params.m_velocity.Get() );
        auto rgPrevAmbientOcclusion = renderGraph.RegisterExternalResource( m_prevAmbientOcclusion.Get() );

        DenoisePassParams denoiseParams = {
            .m_prevImage = rgPrevAmbientOcclusion,
            .m_image = rgAmbientOcclusion,
            .m_prevViewSpaceDistance = rgPrevViewSpaceDistance,
            .m_viewSpaceDistance = rgViewSpaceDistance,
            .m_velocity = rgVelocity,
            .m_kernelRadius = CVarRTAODenoiseKernelRadius.GetInteger(),
            .m_screenSize = Vector2(
                static_cast<float>( params.m_screenWidth ),
                static_cast<float>( params.m_screenHeight )
            ),
        };

        RefHandle<agl::Texture> denoisedAO = AddDenoisePass( renderGraph, denoiseParams );
        m_prevAmbientOcclusion = denoisedAO;

        return denoisedAO;
    }
}
