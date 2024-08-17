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
		virtual Texture* CreateTexture( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData ) const override;

		// Buffer
		virtual Buffer* CreateBuffer( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData ) const override;

		// Shader
		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size ) const override;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual VertexShader* CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual GeometryShader* CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual PixelShader* CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;

		// RenderState
		virtual BlendState* CreateBlendState( const BlendStateTrait& trait ) const override;
		virtual DepthStencilState* CreateDepthStencilState( const DepthStencilStateTrait& trait ) const override;
		virtual RasterizerState* CreateRasterizerState( const RasterizerStateTrait& trait ) const override;
		virtual SamplerState* CreateSamplerState( const SamplerStateTrait& trait ) const override;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) override;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) override;

		// Canvas
		virtual Canvas* CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format ) const override;

		// Viewport
		virtual Viewport* CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) const override;
		virtual Viewport* CreateViewport( Canvas& canvas ) const override;

		virtual GpuTimer* CreateGpuTimer() const override;
		virtual OcclusionQuery* CreateOcclusionQuery() const override;

		virtual void SetPSOCache( std::map<uint64, BinaryChunk>& psoCache ) override;

		CD3D11ResourceManager() = default;
		virtual ~CD3D11ResourceManager() override;
		CD3D11ResourceManager( const CD3D11ResourceManager& ) = delete;
		CD3D11ResourceManager( CD3D11ResourceManager&& ) = delete;
		CD3D11ResourceManager& operator=( const CD3D11ResourceManager& ) = delete;
		CD3D11ResourceManager& operator=( CD3D11ResourceManager&& ) = delete;

	private:
		std::map<GraphicsPipelineStateInitializer, RefHandle<GraphicsPipelineState>> m_graphicsPipelineStateCache;
		std::map<ComputePipelineStateInitializer, RefHandle<ComputePipelineState>> m_computePipelineStateCache;
	};

	Owner<IResourceManager*> CreateD3D11ResourceManager();
}
