#include "VolumetricFogPassProcessor.h"

#include "CommonRenderResource.h"
#include "GlobalShaders.h"
#include "RenderOption.h"
#include "VertexCollection.h"

namespace rendercore
{
	class DrawVolumetricFogPS : public GlobalShaderCommon<PixelShader, DrawVolumetricFogPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( DrawVolumetricFogPS, "./Assets/Shaders/VolumetricFog/PS_DrawVolumetricFog.asset" );

	std::optional<DrawSnapshot> VolumetricFogDrawPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		StaticShaderSwitches switches = DrawVolumetricFogPS::GetSwitches();
		switches.On( Name( "TricubicTextureSampling" ), 1 );

		PassShader passShader = {
			.m_vertexShader = FullScreenQuadVS(),
			.m_geometryShader = nullptr,
			.m_pixelShader = DrawVolumetricFogPS( switches )
		};

		BlendOption volumetricFogDrawPassBlendOption;
		RenderTargetBlendOption& rt0BlendOption = volumetricFogDrawPassBlendOption.m_renderTarget[0];
		rt0BlendOption.m_blendEnable = true;
		rt0BlendOption.m_srcBlend = agl::Blend::One;
		rt0BlendOption.m_destBlend = agl::Blend::SrcAlpha;
		rt0BlendOption.m_srcBlendAlpha = agl::Blend::Zero;
		rt0BlendOption.m_destBlendAlpha = agl::Blend::One;

		DepthStencilOption depthStencilOption;
		depthStencilOption.m_depth.m_enable = false;
		depthStencilOption.m_depth.m_writeDepth = false;

		PassRenderOption passRenderOption = {
			.m_blendOption = &volumetricFogDrawPassBlendOption,
			.m_depthStencilOption = &depthStencilOption
		};

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}
}
