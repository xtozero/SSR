#include "ShadowDrawPassProcessor.h"

#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

namespace rendercore
{
	class DrawCascadeShadowPS
	{
	public:
		DrawCascadeShadowPS();
		PixelShader* Shader() { return m_shader; }

	private:
		PixelShader* m_shader = nullptr;
	};

	REGISTER_GLOBAL_SHADER( DrawCascadeShadowPS, "./Assets/Shaders/Shadow/PS_DrawCascadeShadow.asset" );

	DrawCascadeShadowPS::DrawCascadeShadowPS()
	{
		m_shader = static_cast<PixelShader*>( GetGlobalShader<DrawCascadeShadowPS>()->CompileShader( {} ) );
	}

	std::optional<DrawSnapshot> ShadowDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader{
			FullScreenQuadVS().Shader(),
			nullptr,
			DrawCascadeShadowPS().Shader()
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
