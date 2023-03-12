#pragma once

#include "GuideTypes.h"
#include "IRenderResourceManager.h"

#include <map>

namespace agl
{
	class D3D12ResourceManager final : public IResourceManager
	{
	public:
		virtual void Shutdown() override;

		// Texture
		virtual Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) override;

		// Buffer
		virtual Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) override;

		// Shader
		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size ) override;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;
		virtual VertexShader* CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;
		virtual GeometryShader* CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;
		virtual PixelShader* CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) override;

		// RenderState
		virtual BlendState* CreateBlendState( const BlendStateTrait& trait ) override;
		virtual DepthStencilState* CreateDepthStencilState( const DepthStencilStateTrait& trait ) override;
		virtual RasterizerState* CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) override;
		virtual SamplerState* CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) override;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) override;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) override;

		// Viewport
		virtual Viewport* CreateViewport( uint32 width, uint32 height, void* hWnd, ResourceFormat format ) override;

		D3D12ResourceManager() = default;
		virtual ~D3D12ResourceManager() override;
		D3D12ResourceManager( const D3D12ResourceManager& ) = delete;
		D3D12ResourceManager( D3D12ResourceManager&& ) = delete;
		D3D12ResourceManager& operator=( const D3D12ResourceManager& ) = delete;
		D3D12ResourceManager& operator=( D3D12ResourceManager&& ) = delete;

	private:
		std::map<GraphicsPipelineStateInitializer, RefHandle<GraphicsPipelineState>> m_graphicsPipelineStateCache;
		std::map<ComputePipelineStateInitializer, RefHandle<ComputePipelineState>> m_computePipelineStateCache;
	};

	Owner<IResourceManager*> CreateD3D12ResourceManager();
}
