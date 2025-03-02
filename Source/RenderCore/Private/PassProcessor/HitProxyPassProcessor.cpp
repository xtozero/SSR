#include "HitProxyPassProcessor.h"

#include "GlobalShaders.h"
#include "MaterialResource.h"
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
	{};

	class HitProxyPS final : public GlobalShaderCommon<PixelShader, HitProxyPS>
	{};

	class HitProxyMS final : public GlobalShaderCommon<MeshShader, HitProxyMS>
	{};

	REGISTER_GLOBAL_SHADER( HitProxyVS, "./Assets/Shaders/HitProxy/VS_HitProxy.asset" );
	REGISTER_GLOBAL_SHADER( HitProxyPS, "./Assets/Shaders/HitProxy/PS_HitProxy.asset" );
	REGISTER_GLOBAL_SHADER( HitProxyMS, "./Assets/Shaders/HitProxy/MS_HitProxy.asset" );

	std::optional<DrawSnapshot> HitProxyPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
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
		bool bUseMeshShader = material.UseMeshShader();

		PassShader passShader = {
			.m_vertexShader = bUseMeshShader ? nullptr : HitProxyVS(),
			.m_pixelShader = HitProxyPS(),
			.m_meshShader = bUseMeshShader ? HitProxyMS() : nullptr,
			.m_amplificationShader = bUseMeshShader ? DefaultAS() : nullptr
		};

		return passShader;
	}

	PassProcessorRegister RegisterHitProxyPass( RenderPassType::HitProxy, &CreateHitProxyPassProcessor );
}
