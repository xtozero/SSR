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
	class DefaultLitVS {};
	class DefaultTexturingVS {};
	class SkyboxVS {};

	class DefaultLitPS {};
	class DefaultPBRLitPS {};
	class DefaultTexturingPS {};
	class DefaultUnlitPS {};
	class SkyboxPS {};

	class DefaultLitCS {};
	class DefaultPBRLitCS {};
	class DefaultTexturingCS {};

	class MeshletMS {};
	class TestMS {};

	REGISTER_SHADER( DefaultLitVS, "Material/VS_DefaultLit.fx", agl::ShaderType::VS, "main" );
	REGISTER_SHADER( DefaultTexturingVS, "Material/VS_DefaultTexturing.fx", agl::ShaderType::VS, "main" );
	REGISTER_SHADER( SkyboxVS, "Material/VS_Skybox.fx", agl::ShaderType::VS, "main" );

	REGISTER_SHADER( DefaultLitPS, "Material/PS_DefaultLit.fx", agl::ShaderType::PS, "main" );
	REGISTER_SHADER( DefaultPBRLitPS, "Material/PS_DefaultPBRLit.fx", agl::ShaderType::PS, "main" );
	REGISTER_SHADER( DefaultTexturingPS, "Material/PS_DefaultTexturing.fx", agl::ShaderType::PS, "main" );
	REGISTER_SHADER( DefaultUnlitPS, "Material/PS_DefaultUnlit.fx", agl::ShaderType::PS, "main" );
	REGISTER_SHADER( SkyboxPS, "Material/PS_Skybox.fx", agl::ShaderType::PS, "main" );

	REGISTER_SHADER( DefaultLitCS, "Material/CS_DefaultLit.fx", agl::ShaderType::CS, "main" );
	REGISTER_SHADER( DefaultPBRLitCS, "Material/CS_DefaultPBRLit.fx", agl::ShaderType::CS, "main" );
	REGISTER_SHADER( DefaultTexturingCS, "Material/CS_DefaultTexturing.fx", agl::ShaderType::CS, "main" );

	REGISTER_SHADER( MeshletMS, "Material/MS_Meshlet.fx", agl::ShaderType::MS, "main" );
	REGISTER_SHADER( TestMS, "Material/MS_Test.fx", agl::ShaderType::MS, "main" );

	std::optional<DrawSnapshot> DefaultPassProcessor::ProcessInternal( const PrimitiveSubMesh& subMesh, const PassShader& passShader )
	{
		if ( ( subMesh.m_material == nullptr )
			|| subMesh.m_material->SupportsVisibilityRendering() )
		{
			return {};
		}

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

	PassProcessorRegister RegisterDefaultPass( RenderPassType::Default, &CreateDefaultPassProcessor );
}
