#pragma once

#include "D3D11VetexLayout.h"
#include "GuideTypes.h"
#include "IRenderResourceManager.h"
#include "RefHandle.h"
#include "SizedTypes.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

namespace agl
{
	class CD3D11ResourceManager final : public IResourceManager
	{
	public:
		virtual void Shutdown() override;

		// Texture
		virtual Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;

		// Buffer
		virtual Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) override;

		// Shader
		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size ) override;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, size_t byteCodeSize ) override;
		virtual VertexShader* CreateVertexShader( const void* byteCode, size_t byteCodeSize ) override;
		virtual GeometryShader* CreateGeometryShader( const void* byteCode, size_t byteCodeSize ) override;
		virtual PixelShader* CreatePixelShader( const void* byteCode, size_t byteCodeSize ) override;

		// RenderState
		virtual BlendState* CreateBlendState( const BLEND_STATE_TRAIT& trait ) override;
		virtual DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) override;
		virtual RasterizerState* CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) override;
		virtual SamplerState* CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) override;
		virtual PipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) override;
		virtual PipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) override;

		// Viewport
		virtual Viewport* CreateViewport( uint32 width, uint32 height, void* hWnd, ResourceFormat format ) override;

		CD3D11ResourceManager() = default;
		~CD3D11ResourceManager();
		CD3D11ResourceManager( const CD3D11ResourceManager& ) = delete;
		CD3D11ResourceManager( CD3D11ResourceManager&& ) = delete;
		CD3D11ResourceManager& operator=( const CD3D11ResourceManager& ) = delete;
		CD3D11ResourceManager& operator=( CD3D11ResourceManager&& ) = delete;

	private:
		std::map<GraphicsPipelineStateInitializer, RefHandle<PipelineState>> m_graphicsPipelineStateCache;
		std::map<ComputePipelineStateInitializer, RefHandle<PipelineState>> m_computePipelineStateCache;
	};

	Owner<IResourceManager*> CreateD3D11ResourceManager();
}