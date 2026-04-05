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
	class DepthWriteVS final : public GlobalShaderBase<VertexShader, DepthWriteVS>
	{
		using GlobalShaderBase::GlobalShaderBase;

	public:
		using PermutationType = ShaderPermutation<TAADim>;
	};

	class DepthWritePS final : public GlobalShaderBase<PixelShader, DepthWritePS>
	{};

	class DepthWriteMS final : public GlobalShaderBase<MeshShader, DepthWriteMS>
	{
		using GlobalShaderBase::GlobalShaderBase;

	public:
		using PermutationType = ShaderPermutation<TAADim>;
	};

	REGISTER_GLOBAL_SHADER( DepthWriteVS, "DepthWrite/VS_DepthWrite.fx", "main" );
	REGISTER_GLOBAL_SHADER( DepthWritePS, "DepthWrite/PS_DepthWrite.fx", "main" );
	REGISTER_GLOBAL_SHADER( DepthWriteMS, "DepthWrite/MS_DepthWrite.fx", "main" );

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
		DepthWriteVS::PermutationType vsPermutation;
		DepthWriteMS::PermutationType msPermutation;

		vsPermutation.SetValue<TAADim>( DefaultRenderCore::IsTaaEnabled() );
		msPermutation.SetValue<TAADim>( DefaultRenderCore::IsTaaEnabled() );

		bool bUseMeshShader = material.UseMeshShader();

		PassShader passShader = {
			.m_vertexShader = bUseMeshShader ? nullptr : DepthWriteVS( vsPermutation ),
			.m_pixelShader = DepthWritePS(),
			.m_meshShader = bUseMeshShader ? DepthWriteMS( msPermutation ) : nullptr,
			.m_amplificationShader = bUseMeshShader ? DefaultAS() : nullptr,
		};

		return passShader;
	}

	PassProcessorRegister RegisterDepthWritePass( RenderPassType::DepthWrite, &CreateDepthWritePassProcessor );
}
