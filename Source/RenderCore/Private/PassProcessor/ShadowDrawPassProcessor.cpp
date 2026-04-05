#include "ShadowDrawPassProcessor.h"

#include "CommonRenderResource.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

namespace rendercore
{
	class EnableESMDim : DEFINE_BOOL_DIMENSION( "EnableESMs" );

	class DrawCascadeShadowPS final : public GlobalShaderBase<PixelShader, DrawCascadeShadowPS>
	{
		using GlobalShaderBase::GlobalShaderBase;

	public:
		using PermutationType = ShaderPermutation<EnableESMDim>;
	};

	REGISTER_GLOBAL_SHADER( DrawCascadeShadowPS, "Shadow/PS_DrawCascadeShadow.fx", "main" );

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
		FullScreenQuadVS::PermutationType vsPermutation;
		vsPermutation.SetValue<TAADim>( DefaultRenderCore::IsTaaEnabled() );

		DrawCascadeShadowPS::PermutationType psPermutation;
		psPermutation.SetValue<EnableESMDim>( DefaultRenderCore::IsESMsEnabled() );

		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS( vsPermutation ),
			.m_pixelShader = DrawCascadeShadowPS( psPermutation )
		};

		return passShader;
	}

	class DrawPointShadowPS final : public GlobalShaderBase<PixelShader, DrawPointShadowPS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	REGISTER_GLOBAL_SHADER( DrawPointShadowPS, "Shadow/PS_DrawPointShadow.fx", "main" );

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
		FullScreenQuadVS::PermutationType vsPermutation;
		vsPermutation.SetValue<TAADim>( DefaultRenderCore::IsTaaEnabled()  );

		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS( vsPermutation ),
			.m_pixelShader = DrawPointShadowPS()
		};

		return passShader;
	}
}
