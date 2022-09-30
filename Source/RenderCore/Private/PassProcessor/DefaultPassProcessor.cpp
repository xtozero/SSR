#include "DefaultPassProcessor.h"

#include "MaterialResource.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace
{
	rendercore::IPassProcessor* CreateDefaultPassProcessor()
	{
		return new rendercore::DefaultPassProcessor();
	}
}

namespace rendercore
{
	std::optional<DrawSnapshot> DefaultPassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

		MaterialResource& material = *subMesh.m_material;

		PassShader passShader = CollectPassShader( material );

		PassRenderOption passRenderOption;
		DepthStencilOption depthStencilOption;
		if ( const RenderOption* option = subMesh.m_renderOption )
		{
			if ( option->m_blendOption )
			{
				passRenderOption.m_blendOption = &( *option->m_blendOption );
			}

			if ( option->m_depthStencilOption )
			{
				depthStencilOption = *option->m_depthStencilOption;

				passRenderOption.m_depthStencilOption = &depthStencilOption;
			}

			if ( option->m_rasterizerOption )
			{
				passRenderOption.m_rasterizerOption = &( *option->m_rasterizerOption );
			}
		}

		depthStencilOption.m_depth.m_depthFunc = agl::ComparisonFunc::LessEqual;
		depthStencilOption.m_depth.m_writeDepth = false;
		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::Default );
	}

	PassProcessorRegister RegisterDefaultPass( RenderPass::Default, &CreateDefaultPassProcessor );
}
