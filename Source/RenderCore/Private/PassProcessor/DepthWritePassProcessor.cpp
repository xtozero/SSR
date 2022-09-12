#include "stdafx.h"
#include "DepthWritePassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "GlobalShaders.h"
#include "PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

using namespace rendercore;

namespace
{
	IPassProcessor* CreateDepthWritePassProcessor()
	{
		return new DepthWritePassProcessor();
	}
}

namespace rendercore
{
	class DepthWriteVS
	{
	public:
		DepthWriteVS();
		VertexShader* Shader() { return m_shader; }

	private:
		VertexShader* m_shader = nullptr;
	};

	class DepthWritePS
	{
	public:
		DepthWritePS();
		PixelShader* Shader() { return m_shader; }

	private:
		PixelShader* m_shader = nullptr;
	};

	REGISTER_GLOBAL_SHADER( DepthWriteVS, "./Assets/Shaders/VS_DepthWrite.asset" );
	REGISTER_GLOBAL_SHADER( DepthWritePS, "./Assets/Shaders/PS_DepthWrite.asset" );

	DepthWriteVS::DepthWriteVS()
	{
		m_shader = static_cast<VertexShader*>( GetGlobalShader<DepthWriteVS>()->CompileShader( {} ) );
	}

	DepthWritePS::DepthWritePS()
	{
		m_shader = static_cast<PixelShader*>( GetGlobalShader<DepthWritePS>()->CompileShader( {} ) );
	}

	std::optional<DrawSnapshot> DepthWritePassProcessor::Process( const PrimitiveSubMesh& subMesh )
	{
		assert( IsInRenderThread() );

		PassShader passShader{
			DepthWriteVS().Shader(),
			nullptr,
			DepthWritePS().Shader()
		};

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

	PassProcessorRegister RegisterDepthWritePass( RenderPass::DepthWrite, &CreateDepthWritePassProcessor );
}
