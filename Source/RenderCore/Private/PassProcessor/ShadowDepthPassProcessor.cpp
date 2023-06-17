#include "stdafx.h"
#include "ShadowDepthPassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "GlobalShaders.h"
#include "PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace
{
	rendercore::IPassProcessor* CreateShadowDepthPassProcessor()
	{
		return new rendercore::ShadowDepthPassProcessor();
	}
}

namespace rendercore
{
	class ShadowDepthVS : public GlobalShaderCommon<VertexShader, ShadowDepthVS>
	{};

	class ShadowDepthGS : public GlobalShaderCommon<GeometryShader, ShadowDepthGS>
	{};

	class ShadowDepthPS : public GlobalShaderCommon<PixelShader, ShadowDepthPS>
	{};

	REGISTER_GLOBAL_SHADER( ShadowDepthVS, "./Assets/Shaders/Shadow/VS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( ShadowDepthGS, "./Assets/Shaders/Shadow/GS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( ShadowDepthPS, "./Assets/Shaders/Shadow/PS_CascadedShadowmap.asset" );

	std::optional<DrawSnapshot> ShadowDepthPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

		PassShader passShader{
			ShadowDepthVS().GetShader(),
			ShadowDepthGS().GetShader(),
			ShadowDepthPS().GetShader()
		};

		PassRenderOption passRenderOption;
		if ( const RenderOption* option = subMesh.m_renderOption )
		{
			if ( option->m_blendOption )
			{
				passRenderOption.m_blendOption = &( *option->m_blendOption );
			}

			if ( option->m_rasterizerOption )
			{
				passRenderOption.m_rasterizerOption = &( *option->m_rasterizerOption );
			}
		}

		DepthStencilOption shadowDepthPassDepthStencilOption;
		passRenderOption.m_depthStencilOption = &shadowDepthPassDepthStencilOption;

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionOnly );
	}

	PassProcessorRegister RegisterShadowDepthPass( RenderPass::CSMShadowDepth, &CreateShadowDepthPassProcessor );
}
