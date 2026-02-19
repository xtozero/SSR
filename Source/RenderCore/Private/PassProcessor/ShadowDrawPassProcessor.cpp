#include "ShadowDrawPassProcessor.h"

#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

namespace rendercore
{
	class DrawCascadeShadowPS final : public GlobalShaderCommon<PixelShader, DrawCascadeShadowPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( DrawCascadeShadowPS, "Shadow/PS_DrawCascadeShadow.fx", agl::ShaderType::Pixel, "main" );

	std::optional<DrawSnapshot> CascadeShadowDrawPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		BlendOption shadowDrawPassBlendOption;
		RenderTargetBlendOption& rt0BlendOption = shadowDrawPassBlendOption.m_renderTarget[0];
		rt0BlendOption.m_blendEnable = true;
		rt0BlendOption.m_srcBlend = agl::Blend::Zero;
		rt0BlendOption.m_destBlend = agl::Blend::SrcColor;
		rt0BlendOption.m_srcBlendAlpha = agl::Blend::Zero;
		rt0BlendOption.m_destBlendAlpha = agl::Blend::One;

		DepthStencilOption shadowDrawPassDepthOption;
		shadowDrawPassDepthOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_blendOption = &shadowDrawPassBlendOption,
			.m_depthStencilOption = &shadowDrawPassDepthOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassShader CascadeShadowDrawPassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		StaticShaderSwitches vsSwitches = FullScreenQuadVS::GetSwitches();
		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			vsSwitches.On( StaticName( "TAA" ), 1 );
		}

		StaticShaderSwitches psSwitches = DrawCascadeShadowPS::GetSwitches();
		if ( DefaultRenderCore::IsESMsEnabled() )
		{
			psSwitches.On( StaticName( "EnableESMs" ), 1 );
		}

		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS( vsSwitches ),
			.m_pixelShader = DrawCascadeShadowPS( psSwitches )
		};

		return passShader;
	}

	class DrawPointShadowPS final : public GlobalShaderCommon<PixelShader, DrawPointShadowPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( DrawPointShadowPS, "Shadow/PS_DrawPointShadow.fx", agl::ShaderType::Pixel, "main" );

	std::optional<DrawSnapshot> PointShadowDrawPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		BlendOption shadowDrawPassBlendOption;
		RenderTargetBlendOption& rt0BlendOption = shadowDrawPassBlendOption.m_renderTarget[0];
		rt0BlendOption.m_blendEnable = true;
		rt0BlendOption.m_srcBlend = agl::Blend::Zero;
		rt0BlendOption.m_destBlend = agl::Blend::SrcColor;
		rt0BlendOption.m_srcBlendAlpha = agl::Blend::Zero;
		rt0BlendOption.m_destBlendAlpha = agl::Blend::One;

		DepthStencilOption shadowDrawPassDepthOption;
		shadowDrawPassDepthOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_blendOption = &shadowDrawPassBlendOption,
			.m_depthStencilOption = &shadowDrawPassDepthOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassShader PointShadowDrawPassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		StaticShaderSwitches vsSwitches = FullScreenQuadVS::GetSwitches();
		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			vsSwitches.On( StaticName( "TAA" ), 1 );
		}

		StaticShaderSwitches psSwitches = DrawPointShadowPS::GetSwitches();

		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS( vsSwitches ),
			.m_pixelShader = DrawPointShadowPS( psSwitches )
		};

		return passShader;
	}
}
