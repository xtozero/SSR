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
	class SSGIPassCS final : public GlobalShaderCommon<ComputeShader, SSGIPassCS>
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

	class SSGIDenoiseCS final : public GlobalShaderCommon<ComputeShader, SSGIDenoiseCS>
	{
		DEFINE_SHADER_PARAM( PrevSSGI );
		DEFINE_SHADER_PARAM( SSGI );
		DEFINE_SHADER_PARAM( PrevViewSpaceDistance );
		DEFINE_SHADER_PARAM( ViewSpaceDistance );
		DEFINE_SHADER_PARAM( VelocityTex );

		DEFINE_SHADER_PARAM( BlackBorderSampler );
		
		DEFINE_SHADER_PARAM( DenoisedSSGI );

		DEFINE_SHADER_PARAM( KernelRadius );
		DEFINE_SHADER_PARAM( ScreenSize );
		DEFINE_SHADER_PARAM( InvScreenSize );
	};

	class SSGICompositeVS final : public GlobalShaderCommon<VertexShader, SSGICompositeVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class SSGICompositePS final : public GlobalShaderCommon<PixelShader, SSGICompositePS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( SSGIPassCS, "./Assets/Shaders/SSGI/CS_SSGI.asset" );
	REGISTER_GLOBAL_SHADER( SSGIDenoiseCS, "./Assets/Shaders/SSGI/CS_DenoiseSSGI.asset" );
	REGISTER_GLOBAL_SHADER( SSGICompositeVS, "./Assets/Shaders/SSGI/VS_SSGIComposite.asset" );
	REGISTER_GLOBAL_SHADER( SSGICompositePS, "./Assets/Shaders/SSGI/PS_SSGIComposite.asset" );

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
			vsSwitches.On( Name( "TAA" ), 1 );
		}

		if ( agl::DefaultAgl::IsSupportsBindless() )
		{
			psSwitches.On( Name( "SupportsBindless" ), 1 );
		}

		if ( useDiffuseTexture )
		{
			vsSwitches.On( Name( "UseDiffuseTexture" ), 1  );
			psSwitches.On( Name( "UseDiffuseTexture" ), 1 );
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

	RefHandle<agl::Texture> SSGIRenderer::Render( RenderGraph& renderGraph, const SSGIRenderParam& param )
	{
		CPU_PROFILE( SSGI );
		GPU_PROFILE_EVENT( renderGraph, SSGI );

		auto rgSceneColor = renderGraph.RegisterExternalResource( param.m_sceneColor.Get() );
		auto rgViewSpaceDistance = renderGraph.RegisterExternalResource( param.m_viewSpaceDistance.Get() );
		auto rgWorldNormal = renderGraph.RegisterExternalResource( param.m_worldNormal.Get() );

		const agl::TextureTrait& sceneTexTrait = param.m_sceneColor->GetTrait();

		agl::TextureTrait ssgiTrait = {
			.m_width = sceneTexTrait.m_width,
			.m_height = sceneTexTrait.m_height,
			.m_depth = sceneTexTrait.m_depth,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::RandomAccess | agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::None,
		};
		
		auto rgSSGITex = renderGraph.CreateTexture( ssgiTrait, "SSGITex" );

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
			.m_ssgi = rgSSGITex
		};

		Vector2 screenSize = { 
			static_cast<float>( sceneTexTrait.m_width ),
			static_cast<float>( sceneTexTrait.m_height ) 
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

		BEGIN_RG_RESOURCE_STRUCT( SSGIDenoiseResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( prevSSGI )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( ssgi )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( prevViewSpaceDistance )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( viewSpeaceDistance )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( velocity )
			DECLARE_RG_TEXTURE_UAV( denoisedSSGI )
		END_RG_RESOURCE_STRUCT();
		
		auto rgPrevSSGITex = renderGraph.RegisterExternalResource( m_prevSSGI.Get() );
		auto rgVelocityTex = renderGraph.RegisterExternalResource( param.m_velocity.Get() );
		auto rgPrevViewSpaceDistanceTex = renderGraph.RegisterExternalResource( param.m_prevViewSpaceDistance.Get() );
		auto rgViewSpaceDistanceTex = renderGraph.RegisterExternalResource( param.m_viewSpaceDistance.Get() );
		auto denoisedSSGITex = GraphicsResourcePool::GetInstance().FindFreeTexture( ssgiTrait, "DenoisedSSGITex" );
		auto rgDenoisedSSGITex = renderGraph.RegisterExternalResource( denoisedSSGITex.Get() );
		
		SSGIDenoiseResource ssgiDenoiseResource = {
			.m_prevSSGI = rgPrevSSGITex,
			.m_ssgi = rgSSGITex,
			.m_prevViewSpaceDistance = rgPrevViewSpaceDistanceTex,
			.m_viewSpeaceDistance = rgViewSpaceDistanceTex,
			.m_velocity = rgVelocityTex,
			.m_denoisedSSGI = rgDenoisedSSGITex 
		};

		int kernelRadius = param.m_denoiseKernelRadius;
		
		renderGraph.AddPass( ssgiDenoiseResource,
			[ssgiDenoiseResource, kernelRadius, screenSize]( ComputeCommandList& commandList )
			{
				SSGIDenoiseCS ssgiDenoiseCS;

				RefHandle<agl::ComputePipelineState> ssgiDenoisePSO = PrepareComputePipelineState( ssgiDenoiseCS );
				commandList.BindPipelineState( ssgiDenoisePSO.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( ssgiDenoiseCS );

				BindResource( shaderBindings, ssgiDenoiseCS.PrevSSGI(), ssgiDenoiseResource.m_prevSSGI->Get() );
				BindResource( shaderBindings, ssgiDenoiseCS.SSGI(), ssgiDenoiseResource.m_ssgi->Get() );
				BindResource( shaderBindings, ssgiDenoiseCS.PrevViewSpaceDistance(), ssgiDenoiseResource.m_prevViewSpaceDistance->Get() );
				BindResource( shaderBindings, ssgiDenoiseCS.ViewSpaceDistance(), ssgiDenoiseResource.m_viewSpeaceDistance->Get() );
				BindResource( shaderBindings, ssgiDenoiseCS.VelocityTex(), ssgiDenoiseResource.m_velocity->Get() );
				BindResource( shaderBindings, ssgiDenoiseCS.DenoisedSSGI(), ssgiDenoiseResource.m_denoisedSSGI->Get() );

				SamplerState blackBorderSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, 0.f
					, agl::ComparisonFunc::Never
					, Color( 0, 0, 0, 255 )>::Get();
				BindResource( shaderBindings, ssgiDenoiseCS.BlackBorderSampler(), blackBorderSampler );
				
				SetShaderValue( commandList, ssgiDenoiseCS.KernelRadius(), kernelRadius );
				SetShaderValue( commandList, ssgiDenoiseCS.ScreenSize(), screenSize );
				SetShaderValue( commandList, ssgiDenoiseCS.InvScreenSize(), Vector2::OneVector / screenSize );

				commandList.BindShaderResources( shaderBindings );
				
				auto numThreadGroupX = static_cast<uint32>( std::ceilf( screenSize.x / 8 ) );
				auto numThreadGroupY = static_cast<uint32>( std::ceilf( screenSize.y / 8 ) );
				commandList.Dispatch( numThreadGroupX, numThreadGroupY, 1 );
			} );

		m_prevSSGI = denoisedSSGITex;
		return denoisedSSGITex;
	}
}
