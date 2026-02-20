#include "VolumetricCloudPassProcessor.h"

#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

namespace rendercore
{
	class DrawVolumetricCloudPS final : public GlobalShaderBase<PixelShader, DrawVolumetricCloudPS>
	{};

	REGISTER_GLOBAL_SHADER( DrawVolumetricCloudPS, "Cloud/PS_DrawVolumetricCloud.fx", "main" );

	std::optional<DrawSnapshot> VolumetricCloundDrawPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		BlendOption volumetricCloundDrawPassBlendOption;
		RenderTargetBlendOption& rt0BlendOption = volumetricCloundDrawPassBlendOption.m_renderTarget[0];
		rt0BlendOption.m_blendEnable = true;
		rt0BlendOption.m_srcBlend = agl::Blend::One;
		rt0BlendOption.m_destBlend = agl::Blend::InvSrcAlpha;
		rt0BlendOption.m_srcBlendAlpha = agl::Blend::Zero;
		rt0BlendOption.m_destBlendAlpha = agl::Blend::One;

		DepthStencilOption depthStencilOption;
		depthStencilOption.m_depth.m_enable = false;
		depthStencilOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_blendOption = &volumetricCloundDrawPassBlendOption,
			.m_depthStencilOption = &depthStencilOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassShader VolumetricCloundDrawPassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS(),
			.m_pixelShader = DrawVolumetricCloudPS()
		};

		return passShader;
	}
}
