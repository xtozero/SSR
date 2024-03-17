#include "ShadowDepthPassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace
{
	rendercore::IPassProcessor* CreateCascadeShadowDepthPassProcessor()
	{
		return new rendercore::CascadeShadowDepthPassProcessor();
	}

	rendercore::IPassProcessor* CreatePointShadowDepthPassProcessor()
	{
		return new rendercore::PointShadowDepthPassProcessor();
	}
}

namespace rendercore
{
	class CascadeShadowDepthVS final : public GlobalShaderCommon<VertexShader, CascadeShadowDepthVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class CascadeShadowDepthGS final : public GlobalShaderCommon<GeometryShader, CascadeShadowDepthGS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class CascadeShadowDepthPS final : public GlobalShaderCommon<PixelShader, CascadeShadowDepthPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( CascadeShadowDepthVS, "./Assets/Shaders/Shadow/VS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthGS, "./Assets/Shaders/Shadow/GS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthPS, "./Assets/Shaders/Shadow/PS_CascadedShadowmap.asset" );

	std::optional<DrawSnapshot> CascadeShadowDepthPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

		bool bIsRSMsEnabled = DefaultRenderCore::IsRSMsEnabled();

		StaticShaderSwitches vsSwitches = CascadeShadowDepthVS::GetSwitches();
		StaticShaderSwitches gsSwitches = CascadeShadowDepthGS::GetSwitches();
		StaticShaderSwitches psSwitches = CascadeShadowDepthPS::GetSwitches();

		if ( bIsRSMsEnabled )
		{
			vsSwitches.On( Name( "EnableRSMs" ), 1 );
			gsSwitches.On( Name( "EnableRSMs" ), 1 );
			psSwitches.On( Name( "EnableRSMs" ), 1 );
		}

		PassShader passShader{
			.m_vertexShader = CascadeShadowDepthVS( vsSwitches ),
			.m_geometryShader = CascadeShadowDepthGS( gsSwitches ),
			.m_pixelShader = CascadeShadowDepthPS( psSwitches )
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

	class PointShadowDepthVS final : public GlobalShaderCommon<VertexShader, PointShadowDepthVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class PointShadowDepthGS final : public GlobalShaderCommon<GeometryShader, PointShadowDepthGS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class PointShadowDepthPS final : public GlobalShaderCommon<PixelShader, PointShadowDepthPS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( PointShadowDepthVS, "./Assets/Shaders/Shadow/VS_PointShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthGS, "./Assets/Shaders/Shadow/GS_PointShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthPS, "./Assets/Shaders/Shadow/PS_PointShadowmap.asset" );

	std::optional<DrawSnapshot> PointShadowDepthPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

		PassShader passShader{
			.m_vertexShader = PointShadowDepthVS(),
			.m_geometryShader = PointShadowDepthGS(),
			.m_pixelShader = PointShadowDepthPS()
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

	PassProcessorRegister RegisterCascadeShadowDepthPass( RenderPass::CascadeShadowDepth, &CreateCascadeShadowDepthPassProcessor );
	PassProcessorRegister RegisterPointShadowDepthPass( RenderPass::PointShadowDepth, &CreatePointShadowDepthPassProcessor );
}
