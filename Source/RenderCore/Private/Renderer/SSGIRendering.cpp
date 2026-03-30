#include "SSGIRendering.h"

#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "MaterialResource.h"
#include "PassProcessor.h"
#include "RenderGraph.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "ShaderParameterUtils.h"
#include "VertexCollection.h"

namespace rendercore
{
	class SSGIPassCS final : public GlobalShaderBase<ComputeShader, SSGIPassCS>
	{
		DEFINE_SHADER_PARAM( SceneViewParameters );

		DEFINE_SHADER_PARAM( SceneColor );
		DEFINE_SHADER_PARAM( ViewSpaceDistance );
		DEFINE_SHADER_PARAM( WorldNormal );

		DEFINE_SHADER_PARAM( BlackBorderSampler );

		DEFINE_SHADER_PARAM( SSGI );

		DEFINE_SHADER_PARAM( Thickness );
		DEFINE_SHADER_PARAM( ViewSpaceRadius );
		DEFINE_SHADER_PARAM( NumSlices );
		DEFINE_SHADER_PARAM( NumSteps );
		DEFINE_SHADER_PARAM( ScreenSize );
		DEFINE_SHADER_PARAM( InvScreenSize );
		DEFINE_SHADER_PARAM( ColorIntensity );
	};

	class SSGICompositeVS final : public GlobalShaderBase<VertexShader, SSGICompositeVS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	class SSGICompositePS final : public GlobalShaderBase<PixelShader, SSGICompositePS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	REGISTER_GLOBAL_SHADER( SSGIPassCS, "SSGI/CS_SSGI.fx", "main" );
	REGISTER_GLOBAL_SHADER( SSGICompositeVS, "SSGI/VS_SSGIComposite.fx", "main" );
	REGISTER_GLOBAL_SHADER( SSGICompositePS, "SSGI/PS_SSGIComposite.fx", "main" );

	class ForwardRendererCompositeSSGIPassProcessor final : public IPassProcessor
	{
	protected:
		virtual std::optional<DrawSnapshot> ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader ) override;
		virtual PassShader CollectPassShader( MaterialResource& material ) const override;
	};

	std::optional<DrawSnapshot> ForwardRendererCompositeSSGIPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		DepthStencilOption depthStencilOption;
		depthStencilOption.m_depth.m_depthFunc = agl::ComparisonFunc::LessEqual;
		depthStencilOption.m_depth.m_writeDepth = false;

		BlendOption blendOption;
		blendOption.m_renderTarget[0].m_blendEnable = true;
		blendOption.m_renderTarget[0].m_srcBlend = agl::Blend::One;
		blendOption.m_renderTarget[0].m_destBlend = agl::Blend::One;
		blendOption.m_renderTarget[0].m_blendOp = agl::BlendOp::Add;
		blendOption.m_renderTarget[0].m_srcBlendAlpha = agl::Blend::Zero;
		blendOption.m_renderTarget[0].m_destBlendAlpha = agl::Blend::One;
		blendOption.m_renderTarget[0].m_blendOpAlpha = agl::BlendOp::Add;

		PassRenderOption passRenderOption = {
			.m_blendOption = &blendOption,
			.m_depthStencilOption = &depthStencilOption,
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassShader ForwardRendererCompositeSSGIPassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		bool useDiffuseTexture = false;
		if ( material.GetMaterial() )
		{
			useDiffuseTexture = material.GetMaterial()->HasProperty( "DiffuseTex" );
		}

		StaticShaderSwitches vsSwitches = SSGICompositeVS::GetSwitches();
		StaticShaderSwitches psSwitches = SSGICompositePS::GetSwitches();

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			vsSwitches.On( StaticName( "TAA" ), 1 );
		}

		if ( agl::DefaultAgl::SupportsBindless() )
		{
			psSwitches.On( StaticName( "SupportsBindless" ), 1 );
		}

		if ( useDiffuseTexture )
		{
			vsSwitches.On( StaticName( "UseDiffuseTexture" ), 1 );
			psSwitches.On( StaticName( "UseDiffuseTexture" ), 1 );
		}

		PassShader passShader = {
			.m_vertexShader = SSGICompositeVS( vsSwitches ),
			.m_geometryShader = nullptr,
			.m_pixelShader = SSGICompositePS( psSwitches ),
			.m_meshShader = nullptr,
			.m_amplificationShader = nullptr,
		};

		return passShader;
	}

	IPassProcessor* CreateForwardRendererCompositeSSGIPassProcessor()
	{
		return new ForwardRendererCompositeSSGIPassProcessor();
	}

	PassProcessorRegister RegisterForwardRendererCompositeSSGIPass( RenderPassType::CompositeSSGI, &CreateForwardRendererCompositeSSGIPassProcessor );

	RefHandle<agl::Texture> SSGIRenderPass::Render( RenderGraph& renderGraph, const SSGIRenderParams& param )
	{
		CPU_PROFILE( SSGI );
		GPU_PROFILE_EVENT( renderGraph, SSGI );

		auto rgSceneColor = renderGraph.RegisterExternalResource( param.m_sceneColor.Get() );
		auto rgViewSpaceDistance = renderGraph.RegisterExternalResource( param.m_viewSpaceDistance.Get() );
		auto rgWorldNormal = renderGraph.RegisterExternalResource( param.m_worldNormal.Get() );

		const agl::TextureDesc& sceneTexDesc = param.m_sceneColor->GetDesc();

		agl::TextureDesc ssgiDesc = {
			.m_width = sceneTexDesc.m_width,
			.m_height = sceneTexDesc.m_height,
			.m_depth = sceneTexDesc.m_depth,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
		};

		auto rgSSGI = renderGraph.CreateTexture( ssgiDesc, "SSGITex" );

		BEGIN_RG_RESOURCE_STRUCT( SSGIPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( sceneColor )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( viewSpaceDistance )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( worldNormal )
			DECLARE_RG_TEXTURE_UAV( ssgi )
		END_RG_RESOURCE_STRUCT();

		SSGIPassResource ssgiPassResource = {
			.m_sceneColor = rgSceneColor,
			.m_viewSpaceDistance = rgViewSpaceDistance,
			.m_worldNormal = rgWorldNormal,
			.m_ssgi = rgSSGI
		};

		Vector2 screenSize = {
			static_cast<float>( sceneTexDesc.m_width ),
			static_cast<float>( sceneTexDesc.m_height )
		};

		renderGraph.AddPass( ssgiPassResource,
			[ssgiPassResource, param, screenSize]( ComputeCommandList& commandList )
			{
				SSGIPassCS ssgiPassCS;

				RefHandle<agl::ComputePipelineState> ssgiPSO = PrepareComputePipelineState( ssgiPassCS );
				commandList.BindPipelineState( ssgiPSO.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( ssgiPassCS );

				BindResource( shaderBindings, ssgiPassCS.SceneViewParameters(), param.m_viewShaderArguments.Get() );

				BindResource( shaderBindings, ssgiPassCS.SceneColor(), ssgiPassResource.m_sceneColor->Get() );
				BindResource( shaderBindings, ssgiPassCS.ViewSpaceDistance(), ssgiPassResource.m_viewSpaceDistance->Get() );
				BindResource( shaderBindings, ssgiPassCS.WorldNormal(), ssgiPassResource.m_worldNormal->Get() );

				SamplerState blackBorderSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, 0.f
					, agl::ComparisonFunc::Never
					, Color( 0, 0, 0, 255 )>::Get();
				BindResource( shaderBindings, ssgiPassCS.BlackBorderSampler(), blackBorderSampler );

				BindResource( shaderBindings, ssgiPassCS.SSGI(), ssgiPassResource.m_ssgi->Get() );

				SetShaderValue( commandList, ssgiPassCS.Thickness(), param.m_thickness );
				SetShaderValue( commandList, ssgiPassCS.ViewSpaceRadius(), param.m_viewSpaceRadius );
				SetShaderValue( commandList, ssgiPassCS.NumSlices(), param.m_numSlices );
				SetShaderValue( commandList, ssgiPassCS.NumSteps(), param.m_numSteps );
				SetShaderValue( commandList, ssgiPassCS.ScreenSize(), screenSize );
				SetShaderValue( commandList, ssgiPassCS.InvScreenSize(), Vector2::OneVector / screenSize );
				SetShaderValue( commandList, ssgiPassCS.ColorIntensity(), param.m_colorIntensity );

				commandList.BindShaderResources( shaderBindings );

				auto numThreadGroupX = static_cast<uint32>( std::ceilf( screenSize.x / 8 ) );
				auto numThreadGroupY = static_cast<uint32>( std::ceilf( screenSize.y / 8 ) );
				commandList.Dispatch( numThreadGroupX, numThreadGroupY, 1 );
			} );

		auto rgPrevSSGI = renderGraph.RegisterExternalResource( m_prevSSGI.Get() );
		auto rgVelocity = renderGraph.RegisterExternalResource( param.m_velocity.Get() );
		auto rgPrevViewSpaceDistance = renderGraph.RegisterExternalResource( param.m_prevViewSpaceDistance.Get() );

		DenoisePassParams denoiseParams = {
			.m_prevImage = rgPrevSSGI,
			.m_image = rgSSGI,
			.m_prevViewSpaceDistance = rgPrevViewSpaceDistance,
			.m_viewSpaceDistance = rgViewSpaceDistance,
			.m_velocity = rgVelocity,
			.m_kernelRadius = param.m_denoiseKernelRadius,
			.m_screenSize = screenSize,
		};

		RefHandle<agl::Texture> denoisedSSGI = AddDenoisePass( renderGraph, denoiseParams );
		m_prevSSGI = denoisedSSGI;

		return denoisedSSGI;
	}
}
