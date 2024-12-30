#include "ShadowDepthPassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "MaterialResource.h"
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

	class CascadeShadowDepthMS final : public GlobalShaderCommon<MeshShader, CascadeShadowDepthMS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class CascadeShadowDepthAS final : public GlobalShaderCommon<AmplificationShader, CascadeShadowDepthAS>
	{};

	REGISTER_GLOBAL_SHADER( CascadeShadowDepthVS, "./Assets/Shaders/Shadow/VS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthGS, "./Assets/Shaders/Shadow/GS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthPS, "./Assets/Shaders/Shadow/PS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthMS, "./Assets/Shaders/Shadow/MS_CascadedShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthAS, "./Assets/Shaders/Shadow/AS_CascadedShadowmap.asset" );

	std::optional<DrawSnapshot> CascadeShadowDepthPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
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

		bool bIsRSMsEnabled = DefaultRenderCore::IsRSMsEnabled();
		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, bIsRSMsEnabled ? VertexStreamLayoutType::Default : VertexStreamLayoutType::PositionOnly );
	}

	PassShader CascadeShadowDepthPassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		bool bIsRSMsEnabled = DefaultRenderCore::IsRSMsEnabled();

		StaticShaderSwitches vsSwitches = CascadeShadowDepthVS::GetSwitches();
		StaticShaderSwitches gsSwitches = CascadeShadowDepthGS::GetSwitches();
		StaticShaderSwitches psSwitches = CascadeShadowDepthPS::GetSwitches();
		StaticShaderSwitches msSwitches = CascadeShadowDepthGS::GetSwitches();

		if ( bIsRSMsEnabled )
		{
			vsSwitches.On( Name( "EnableRSMs" ), 1 );
			gsSwitches.On( Name( "EnableRSMs" ), 1 );
			psSwitches.On( Name( "EnableRSMs" ), 1 );
			msSwitches.On( Name( "EnableRSMs" ), 1 );
		}

		bool bUseMeshShader = material.UseMeshShader();

		PassShader passShader = {
			.m_vertexShader = bUseMeshShader ? nullptr : CascadeShadowDepthVS( vsSwitches ),
			.m_geometryShader = bUseMeshShader ? nullptr : CascadeShadowDepthGS( gsSwitches ),
			.m_pixelShader = CascadeShadowDepthPS( psSwitches ),
			.m_meshShader = bUseMeshShader ? CascadeShadowDepthMS( msSwitches ) : nullptr,
			.m_amplificationShader = bUseMeshShader ? CascadeShadowDepthAS() : nullptr
		};

		return passShader;
	}

	class PointShadowDepthVS final : public GlobalShaderCommon<VertexShader, PointShadowDepthVS>
	{};

	class PointShadowDepthGS final : public GlobalShaderCommon<GeometryShader, PointShadowDepthGS>
	{};

	class PointShadowDepthPS final : public GlobalShaderCommon<PixelShader, PointShadowDepthPS>
	{};

	class PointShadowDepthMS final : public GlobalShaderCommon<MeshShader, PointShadowDepthMS>
	{};

	class PointShadowDepthAS final : public GlobalShaderCommon<AmplificationShader, PointShadowDepthAS>
	{};

	REGISTER_GLOBAL_SHADER( PointShadowDepthVS, "./Assets/Shaders/Shadow/VS_PointShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthGS, "./Assets/Shaders/Shadow/GS_PointShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthPS, "./Assets/Shaders/Shadow/PS_PointShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthMS, "./Assets/Shaders/Shadow/MS_PointShadowmap.asset" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthAS, "./Assets/Shaders/Shadow/AS_PointShadowmap.asset" );

	std::optional<DrawSnapshot> PointShadowDepthPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
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

	PassShader PointShadowDepthPassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		bool bUseMeshShader = material.UseMeshShader();

		PassShader passShader = {
			.m_vertexShader = bUseMeshShader ? nullptr : PointShadowDepthVS(),
			.m_geometryShader = bUseMeshShader ? nullptr : PointShadowDepthGS(),
			.m_pixelShader = PointShadowDepthPS(),
			.m_meshShader = bUseMeshShader ? PointShadowDepthMS() : nullptr,
			.m_amplificationShader = bUseMeshShader ? PointShadowDepthAS() : nullptr,
		};

		return passShader;
	}

	PassProcessorRegister RegisterCascadeShadowDepthPass( RenderPass::CascadeShadowDepth, &CreateCascadeShadowDepthPassProcessor );
	PassProcessorRegister RegisterPointShadowDepthPass( RenderPass::PointShadowDepth, &CreatePointShadowDepthPassProcessor );
}
