#include "ShadowDrawPassProcessor.h"

#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

namespace rendercore
{
	class DrawCascadeShadowPS : public GlobalShaderCommon<PixelShader, DrawCascadeShadowPS>
	{};

	REGISTER_GLOBAL_SHADER( DrawCascadeShadowPS, "./Assets/Shaders/Shadow/PS_DrawCascadeShadow.asset" );

	std::optional<DrawSnapshot> ShadowDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		StaticShaderSwitches switches = FullScreenQuadVS().GetSwitches();

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			switches.On( Name( "TAA" ), 1 );
		}

		PassShader passShader{
			FullScreenQuadVS().GetShader( switches ),
			nullptr,
			DrawCascadeShadowPS().GetShader()
		};

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
}
