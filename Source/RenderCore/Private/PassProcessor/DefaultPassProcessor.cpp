#include "DefaultPassProcessor.h"

#include "Config/DefaultAglConfig.h"
#include "Config/DefaultRenderCoreConfig.h"
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

	PassShader DefaultPassProcessor::CollectPassShader( MaterialResource& material ) const
	{
		StaticShaderSwitches vsSwitches = material.GetShaderSwitches( agl::ShaderType::VS );
		StaticShaderSwitches gsSwitches = material.GetShaderSwitches( agl::ShaderType::GS );
		StaticShaderSwitches psSwitches = material.GetShaderSwitches( agl::ShaderType::PS );

		if ( DefaultRenderCore::IsTaaEnabled() )
		{
			vsSwitches.On( Name( "TAA" ), 1 );
		}

		if ( DefaultRenderCore::IsRSMsEnabled() )
		{
			psSwitches.On( Name( "EnableRSMs" ), 1 );
		}

		if ( DefaultRenderCore::UseIrradianceMapSH() )
		{
			psSwitches.On( Name( "UseIrradianceMapSH" ), 1 );
		}

		if ( agl::DefaultAgl::IsSupportsBindless() )
		{
			psSwitches.On( Name( "SupportsBindless" ), 1 );
		}

		PassShader passShader{
			material.GetVertexShader( &vsSwitches ),
			material.GetGeometryShader( &gsSwitches ),
			material.GetPixelShader( &psSwitches )
		};

		return passShader;
	}

	PassProcessorRegister RegisterDefaultPass( RenderPass::Default, &CreateDefaultPassProcessor );
}
