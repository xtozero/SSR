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
	class DefaultLitVS : public ShaderTraits<VertexShader> {};
	class DefaultTexturingVS : public ShaderTraits<VertexShader> {};
	class SkyboxVS : public ShaderTraits<VertexShader> {};

	class DefaultLitPS : public ShaderTraits<PixelShader> {};
	class DefaultPBRLitPS : public ShaderTraits<PixelShader> {};
	class DefaultTexturingPS : public ShaderTraits<PixelShader> {};
	class DefaultUnlitPS : public ShaderTraits<PixelShader> {};
	class SkyboxPS : public ShaderTraits<PixelShader> {};

	class DefaultLitCS : public ShaderTraits<ComputeShader> {};
	class DefaultPBRLitCS : public ShaderTraits<ComputeShader> {};
	class DefaultTexturingCS : public ShaderTraits<ComputeShader> {};

	class MeshletMS : public ShaderTraits<MeshShader> {};
	class TestMS : public ShaderTraits<MeshShader> {};

	REGISTER_SHADER( DefaultLitVS, "Material/VS_DefaultLit.fx", "main" );
	REGISTER_SHADER( DefaultTexturingVS, "Material/VS_DefaultTexturing.fx", "main" );
	REGISTER_SHADER( SkyboxVS, "Material/VS_Skybox.fx", "main" );

	REGISTER_SHADER( DefaultLitPS, "Material/PS_DefaultLit.fx", "main" );
	REGISTER_SHADER( DefaultPBRLitPS, "Material/PS_DefaultPBRLit.fx", "main" );
	REGISTER_SHADER( DefaultTexturingPS, "Material/PS_DefaultTexturing.fx", "main" );
	REGISTER_SHADER( DefaultUnlitPS, "Material/PS_DefaultUnlit.fx", "main" );
	REGISTER_SHADER( SkyboxPS, "Material/PS_Skybox.fx", "main" );

	REGISTER_SHADER( DefaultLitCS, "Material/CS_DefaultLit.fx", "main" );
	REGISTER_SHADER( DefaultPBRLitCS, "Material/CS_DefaultPBRLit.fx", "main" );
	REGISTER_SHADER( DefaultTexturingCS, "Material/CS_DefaultTexturing.fx", "main" );

	REGISTER_SHADER( MeshletMS, "Material/MS_Meshlet.fx", "main" );
	REGISTER_SHADER( TestMS, "Material/MS_Test.fx", "main" );

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
