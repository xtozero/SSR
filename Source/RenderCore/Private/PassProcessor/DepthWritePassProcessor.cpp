#include "DepthWritePassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "MaterialResource.h"
#include "PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace
{
	rendercore::IPassProcessor* CreateDepthWritePassProcessor()
	{
		return new rendercore::DepthWritePassProcessor();
	}
}

namespace rendercore
{
	class DepthWriteVS final : public GlobalShaderCommon<VertexShader, DepthWriteVS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	class DepthWritePS final : public GlobalShaderCommon<PixelShader, DepthWritePS>
	{};

	class DepthWriteMS final : public GlobalShaderCommon<MeshShader, DepthWriteMS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;
	};

	REGISTER_GLOBAL_SHADER( DepthWriteVS, "DepthWrite/VS_DepthWrite.fx", agl::ShaderType::Vertex, "main" );
	REGISTER_GLOBAL_SHADER( DepthWritePS, "DepthWrite/PS_DepthWrite.fx", agl::ShaderType::Pixel, "main" );
	REGISTER_GLOBAL_SHADER( DepthWriteMS, "DepthWrite/MS_DepthWrite.fx", agl::ShaderType::Mesh, "main" );

	std::optional<DrawSnapshot> DepthWritePassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		PassRenderOption passRenderOption;
		if ( const RenderOption* option = subMesh.m_renderOption )
		{
			if ( option->m_blendOption )
			{
				passRenderOption.m_blendOption = &( *option->m_blendOption );
			}

			if ( option->m_depthStencilOption )
			{
				passRenderOption.m_depthStencilOption = &( *option->m_depthStencilOption );
			}

			if ( option->m_rasterizerOption )
			{
				passRenderOption.m_rasterizerOption = &( *option->m_rasterizerOption );
			}
		}

		if ( subMesh.m_vertexCollection )
		{
			const VertexStreamLayout& layout = subMesh.m_vertexCollection->VertexLayout( VertexStreamLayoutType::PositionNormal );
			if ( layout.Size() == 0 )
			{
				return {};
			}
		}

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionNormal );
	}

	PassShader DepthWritePassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		StaticShaderSwitches vsSwitches = DepthWriteVS::GetSwitches();
		StaticShaderSwitches msSwitches = DepthWriteMS::GetSwitches();

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			vsSwitches.On( StaticName( "TAA" ), 1 );
			msSwitches.On( StaticName( "TAA" ), 1 );
		}

		bool bUseMeshShader = material.UseMeshShader();

		PassShader passShader = {
			.m_vertexShader = bUseMeshShader ? nullptr : DepthWriteVS( vsSwitches ),
			.m_pixelShader = DepthWritePS(),
			.m_meshShader = bUseMeshShader ? DepthWriteMS( msSwitches ) : nullptr,
			.m_amplificationShader = bUseMeshShader ? DefaultAS() : nullptr,
		};

		return passShader;
	}

	PassProcessorRegister RegisterDepthWritePass( RenderPassType::DepthWrite, &CreateDepthWritePassProcessor );
}
