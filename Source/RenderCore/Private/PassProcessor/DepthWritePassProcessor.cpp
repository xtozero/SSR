#include "stdafx.h"
#include "DepthWritePassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "GlobalShaders.h"
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
	class DepthWriteVS
	{
	public:
		DepthWriteVS();

		StaticShaderSwitches GetShaderSwitches()
		{
			if ( m_shader )
			{
				return m_shader->GetStaticSwitches();
			}

			return {};
		}

		VertexShader* Shader( const StaticShaderSwitches& switches )
		{
			if ( m_shader )
			{
				return static_cast<VertexShader*>( m_shader->CompileShader( switches ) );
			}

			return nullptr;
		}

	private:
		IShader* m_shader = nullptr;
	};

	class DepthWritePS
	{
	public:
		DepthWritePS();
		PixelShader* Shader( const StaticShaderSwitches& switches )
		{ 
			if ( m_shader )
			{
				return static_cast<PixelShader*>( m_shader->CompileShader( switches ) );
			}

			return nullptr;
		}

	private:
		IShader* m_shader = nullptr;
	};

	REGISTER_GLOBAL_SHADER( DepthWriteVS, "./Assets/Shaders/VS_DepthWrite.asset" );
	REGISTER_GLOBAL_SHADER( DepthWritePS, "./Assets/Shaders/PS_DepthWrite.asset" );

	DepthWriteVS::DepthWriteVS()
	{
		m_shader = GetGlobalShader<DepthWriteVS>();
	}

	DepthWritePS::DepthWritePS()
	{
		m_shader = GetGlobalShader<DepthWritePS>();
	}

	std::optional<DrawSnapshot> DepthWritePassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

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
			const VertexStreamLayout& layout = subMesh.m_vertexCollection->VertexLayout( VertexStreamLayoutType::PositionNormal );
			if ( layout.Size() == 0 )
			{
				return {};
			}
		}

		return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionNormal );
	}

	PassShader DepthWritePassProcessor::CollectPassShader( [[maybe_unused]] MaterialResource& material ) const
	{
		StaticShaderSwitches switches = DepthWriteVS().GetShaderSwitches();
		switches.On( Name( "TAA" ), 1 );

		PassShader passShader{
			DepthWriteVS().Shader( switches ),
			nullptr,
			DepthWritePS().Shader( {} )
		};

		return passShader;
	}

	PassProcessorRegister RegisterDepthWritePass( RenderPass::DepthWrite, &CreateDepthWritePassProcessor );
}
