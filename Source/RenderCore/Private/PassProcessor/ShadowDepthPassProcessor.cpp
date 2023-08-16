#include "stdafx.h"
#include "ShadowDepthPassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "Config/DefaultRenderCoreConfig.h"
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
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class ShadowDepthGS : public GlobalShaderCommon<GeometryShader, ShadowDepthGS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class ShadowDepthPS : public GlobalShaderCommon<PixelShader, ShadowDepthPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( ShadowDepthVS, "./Assets/Shaders/Shadow/VS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( ShadowDepthGS, "./Assets/Shaders/Shadow/GS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( ShadowDepthPS, "./Assets/Shaders/Shadow/PS_CascadedShadowmap.asset" );

	std::optional<DrawSnapshot> ShadowDepthPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

		bool bIsRSMsEnabled = DefaultRenderCore::IsRSMsEnabled();

		StaticShaderSwitches vsSwitches = ShadowDepthVS::GetSwitches();
		StaticShaderSwitches gsSwitches = ShadowDepthGS::GetSwitches();
		StaticShaderSwitches psSwitches = ShadowDepthPS::GetSwitches();

		if ( bIsRSMsEnabled )
		{
			vsSwitches.On( Name( "EnableRSMs" ), 1 );
			gsSwitches.On( Name( "EnableRSMs" ), 1 );
			psSwitches.On( Name( "EnableRSMs" ), 1 );
		}

		PassShader passShader{
			.m_vertexShader = ShadowDepthVS( vsSwitches ),
			.m_geometryShader = ShadowDepthGS( gsSwitches ),
			.m_pixelShader = ShadowDepthPS( psSwitches )
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

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, bIsRSMsEnabled ? VertexStreamLayoutType::Default : VertexStreamLayoutType::PositionOnly );
	}

	PassProcessorRegister RegisterShadowDepthPass( RenderPass::CSMShadowDepth, &CreateShadowDepthPassProcessor );
}
