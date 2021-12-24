#include "ShadowDrawPassProcessor.h"

#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

class DrawCascadeShadowPS
{
public:
	DrawCascadeShadowPS( );
	PixelShader* Shader( ) { return m_shader; }

private:
	PixelShader* m_shader = nullptr;
};

REGISTER_GLOBAL_SHADER( DrawCascadeShadowPS, "./Assets/Shaders/PS_DrawCascadeShadow.asset" );

DrawCascadeShadowPS::DrawCascadeShadowPS( )
{
	m_shader = static_cast<PixelShader*>( GetGlobalShader<DrawCascadeShadowPS>( ) );
}

std::optional<DrawSnapshot> ShadowDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
{
	PassShader passShader{
		FullScreenQuadVS( ).Shader( ),
		nullptr,
		DrawCascadeShadowPS( ).Shader( )
	};

	PassRenderOption passRenderOption;

	BlendOption shadowDrawPassBlendOption;
	RenderTargetBlendOption& rt0BlendOption = shadowDrawPassBlendOption.m_renderTarget[0];
	rt0BlendOption.m_blendEnable = true;
	rt0BlendOption.m_srcBlend = BLEND::ZERO;
	rt0BlendOption.m_destBlend = BLEND::SRC_COLOR;
	rt0BlendOption.m_srcBlendAlpha = BLEND::ZERO;
	rt0BlendOption.m_destBlendAlpha = BLEND::ONE;

	passRenderOption.m_blendOption = &shadowDrawPassBlendOption;

	return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
}
