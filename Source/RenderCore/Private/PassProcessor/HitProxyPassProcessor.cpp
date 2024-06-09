#include "HitProxyPassProcessor.h"

#include "GlobalShaders.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace
{
	rendercore::IPassProcessor* CreateHitProxyPassProcessor()
	{
		return new rendercore::HitProxyPassProcessor();
	}
}

namespace rendercore
{
	class HitProxyVS final : public GlobalShaderCommon<VertexShader, HitProxyVS>
	{
	};

	class HitProxyPS final : public GlobalShaderCommon<PixelShader, HitProxyPS>
	{
	};

	REGISTER_GLOBAL_SHADER( HitProxyVS, "./Assets/Shaders/HitProxy/VS_HitProxy.asset" );
	REGISTER_GLOBAL_SHADER( HitProxyPS, "./Assets/Shaders/HitProxy/PS_HitProxy.asset" );

	std::optional<DrawSnapshot> HitProxyPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		PassShader passShader = CollectPassShader( *subMesh.m_material );

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
			const VertexStreamLayout& layout = subMesh.m_vertexCollection->VertexLayout( VertexStreamLayoutType::PositionOnly );
			if ( layout.Size() == 0 )
			{
				return {};
			}
		}

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionOnly );
	}

	PassShader HitProxyPassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		PassShader passShader{
			.m_vertexShader = HitProxyVS(),
			.m_pixelShader = HitProxyPS(),
		};

		return passShader;
	}

	PassProcessorRegister RegisterHitProxyPass( RenderPass::HitProxy, &CreateHitProxyPassProcessor );
}
