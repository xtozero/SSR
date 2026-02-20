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
	class CascadeShadowDepthVS final : public GlobalShaderBase<VertexShader, CascadeShadowDepthVS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	class CascadeShadowDepthGS final : public GlobalShaderBase<GeometryShader, CascadeShadowDepthGS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	class CascadeShadowDepthPS final : public GlobalShaderBase<PixelShader, CascadeShadowDepthPS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	class CascadeShadowDepthMS final : public GlobalShaderBase<MeshShader, CascadeShadowDepthMS>
	{
		using GlobalShaderBase::GlobalShaderBase;
	};

	class CascadeShadowDepthAS final : public GlobalShaderBase<AmplificationShader, CascadeShadowDepthAS>
	{};

	REGISTER_GLOBAL_SHADER( CascadeShadowDepthVS, "Shadow/VS_CascadedShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthGS, "Shadow/GS_CascadedShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthPS, "Shadow/PS_CascadedShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthMS, "Shadow/MS_CascadedShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( CascadeShadowDepthAS, "Shadow/AS_CascadedShadowmap.fx", "main" );

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
			vsSwitches.On( StaticName( "EnableRSMs" ), 1 );
			gsSwitches.On( StaticName( "EnableRSMs" ), 1 );
			psSwitches.On( StaticName( "EnableRSMs" ), 1 );
			msSwitches.On( StaticName( "EnableRSMs" ), 1 );
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

	class PointShadowDepthVS final : public GlobalShaderBase<VertexShader, PointShadowDepthVS>
	{};

	class PointShadowDepthGS final : public GlobalShaderBase<GeometryShader, PointShadowDepthGS>
	{};

	class PointShadowDepthPS final : public GlobalShaderBase<PixelShader, PointShadowDepthPS>
	{};

	class PointShadowDepthMS final : public GlobalShaderBase<MeshShader, PointShadowDepthMS>
	{};

	class PointShadowDepthAS final : public GlobalShaderBase<AmplificationShader, PointShadowDepthAS>
	{};

	REGISTER_GLOBAL_SHADER( PointShadowDepthVS, "Shadow/VS_PointShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthGS, "Shadow/GS_PointShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthPS, "Shadow/PS_PointShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthMS, "Shadow/MS_PointShadowmap.fx", "main" );
	REGISTER_GLOBAL_SHADER( PointShadowDepthAS, "Shadow/AS_PointShadowmap.fx", "main" );

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

	PassProcessorRegister RegisterCascadeShadowDepthPass( RenderPassType::CascadeShadowDepth, &CreateCascadeShadowDepthPassProcessor );
	PassProcessorRegister RegisterPointShadowDepthPass( RenderPassType::PointShadowDepth, &CreatePointShadowDepthPassProcessor );
}
