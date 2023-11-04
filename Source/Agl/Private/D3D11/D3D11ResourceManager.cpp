#include "stdafx.h"
#include "D3D11ResourceManager.h"

#include "Buffer.h"

#include "common.h"

#include "D3D11BaseTexture.h"
#include "D3D11BlendState.h"
#include "D3D11Buffer.h"
#include "D3D11DepthStencilState.h"
#include "D3D11FlagConvertor.h"
#include "D3D11PipelineState.h"
#include "D3D11RasterizerState.h"
#include "D3D11SamplerState.h"
#include "D3D11Shaders.h"
#include "D3D11VetexLayout.h"
#include "D3D11Viewport.h"

#include "DxgiSwapchain.h"

#include "EnumStringMap.h"

#include "Texture.h"

#include <cstddef>
#include <fstream>

namespace agl
{
	void CD3D11ResourceManager::Shutdown()
	{
		m_graphicsPipelineStateCache.clear();
	}

	Texture* CD3D11ResourceManager::CreateTexture( const TextureTrait& trait, const char* debugName, const ResourceInitData* initData )
	{
		Texture* newTexture = nullptr;
		if ( IsTexture1D( trait ) )
		{
			newTexture = new D3D11BaseTexture1D( trait, debugName, initData );
		}
		else if ( IsTexture2D( trait ) )
		{
			newTexture = new D3D11BaseTexture2D( trait, debugName, initData );
		}
		else if ( IsTexture3D( trait ) )
		{
			newTexture = new D3D11BaseTexture3D( trait, debugName, initData );
		}
		else
		{
			return nullptr;
		}

		return newTexture;
	}

	Buffer* CD3D11ResourceManager::CreateBuffer( const BufferTrait& trait, const char* debugName, const void* initData )
	{
		Buffer* newBuffer = new D3D11Buffer( trait, debugName, initData );

		return newBuffer;
	}

	VertexLayout* CD3D11ResourceManager::CreateVertexLayout( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size )
	{
		auto d3d11VS = static_cast<const D3D11VertexShader*>( vs );
		auto newVertexLayout = new D3D11VertexLayout( d3d11VS, trait, size );

		return newVertexLayout;
	}

	ComputeShader * CD3D11ResourceManager::CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = new D3D11ComputeShader( byteCode, byteCodeSize, paramInfo );

		return newShader;
	}

	VertexShader* CD3D11ResourceManager::CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = new D3D11VertexShader( byteCode, byteCodeSize, paramInfo );

		return newShader;
	}

	GeometryShader* CD3D11ResourceManager::CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = new D3D11GeometryShader( byteCode, byteCodeSize, paramInfo );

		return newShader;
	}

	PixelShader* CD3D11ResourceManager::CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo )
	{
		auto newShader = new D3D11PixelShader( byteCode, byteCodeSize, paramInfo );

		return newShader;
	}

	BlendState* CD3D11ResourceManager::CreateBlendState( const BlendStateTrait& trait )
	{
		auto blendState = new D3D11BlendState( trait );

		return blendState;
	}

	DepthStencilState* CD3D11ResourceManager::CreateDepthStencilState( const DepthStencilStateTrait& trait )
	{
		auto depthStencilState = new D3D11DepthStencilState( trait );

		return depthStencilState;
	}

	RasterizerState* CD3D11ResourceManager::CreateRasterizerState( const RasterizerStateTrait& trait )
	{
		auto rasterizerState = new D3D11RasterizerState( trait );

		return rasterizerState;
	}

	SamplerState* CD3D11ResourceManager::CreateSamplerState( const SamplerStateTrait& trait )
	{
		auto samplerState = new D3D11SamplerState( trait );

		return samplerState;
	}

	GraphicsPipelineState* CD3D11ResourceManager::CreatePipelineState( const GraphicsPipelineStateInitializer& initializer )
	{
		auto cached = m_graphicsPipelineStateCache.find( initializer );
		if ( cached != m_graphicsPipelineStateCache.end() )
		{
			return cached->second;
		}

		auto pipelineState = new D3D11GraphicsPipelineState( initializer );
		m_graphicsPipelineStateCache.emplace( initializer, pipelineState );

		return pipelineState;
	}

	ComputePipelineState* CD3D11ResourceManager::CreatePipelineState( const ComputePipelineStateInitializer& initializer )
	{
		auto cached = m_computePipelineStateCache.find( initializer );
		if ( cached != m_computePipelineStateCache.end() )
		{
			return cached->second;
		}

		auto pipelineState = new D3D11ComputePipelineState( initializer );
		m_computePipelineStateCache.emplace( initializer, pipelineState );

		return pipelineState;
	}

	Canvas* CD3D11ResourceManager::CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format )
	{
		return new DxgiSwapchain<AglType::D3D11>( D3D11Device(), D3D11Factory(), width, height, 1, hWnd, ConvertFormatToDxgiFormat( format ) );
	}

	Viewport* CD3D11ResourceManager::CreateViewport( uint32 width, uint32 height, ResourceFormat format, [[maybe_unused]] const float4& bgColor )
	{
		auto viewport = new D3D11Viewport( width, height, ConvertFormatToDxgiFormat( format ) );

		return viewport;
	}

	Viewport* CD3D11ResourceManager::CreateViewport( Canvas& canvas )
	{
		return new D3D11Viewport( *reinterpret_cast<DxgiSwapchain<AglType::D3D11>*>( &canvas ) );
	}

	CD3D11ResourceManager::~CD3D11ResourceManager()
	{
		Shutdown();
	}

	Owner<IResourceManager*> CreateD3D11ResourceManager()
	{
		return new CD3D11ResourceManager();
	}
}