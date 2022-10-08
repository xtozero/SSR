#include "VolumetricCloudPassProcessor.h"

#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

namespace rendercore
{
	class DrawVolumetricCloudPS : public GlobalShaderCommon<PixelShader, DrawVolumetricCloudPS>
	{};

	REGISTER_GLOBAL_SHADER( DrawVolumetricCloudPS, "./Assets/Shaders/Cloud/PS_DrawVolumetricCloud.asset" );

	std::optional<DrawSnapshot> VolumetricCloundDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader{
					FullScreenQuadVS().GetShader(),
					nullptr,
					DrawVolumetricCloudPS().GetShader()
		};

		BlendOption volumetricCloundDrawPassBlendOption;
		RenderTargetBlendOption& rt0BlendOption = volumetricCloundDrawPassBlendOption.m_renderTarget[0];
		rt0BlendOption.m_blendEnable = true;
		rt0BlendOption.m_srcBlend = agl::Blend::One;
		rt0BlendOption.m_destBlend = agl::Blend::InvSrcAlpha;
		rt0BlendOption.m_srcBlendAlpha = agl::Blend::Zero;
		rt0BlendOption.m_destBlendAlpha = agl::Blend::One;

		PassRenderOption passRenderOption = {
			.m_blendOption = &volumetricCloundDrawPassBlendOption,
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}
}
