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
	class D3D11ResourceManager final : public IResourceManager
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
		virtual MeshShader* CreateMeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual AmplificationShader* CreateAmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual RayGenerationShader* CreateRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual IntersectionShader* CreateIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual AnyHitShader* CreateAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual ClosestHitShader* CreateClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual MissShader* CreateMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;
		virtual CallableShader* CreateCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const override;

		// RenderState
		virtual BlendState* CreateBlendState( const BlendStateTrait& trait ) const override;
		virtual DepthStencilState* CreateDepthStencilState( const DepthStencilStateTrait& trait ) const override;
		virtual RasterizerState* CreateRasterizerState( const RasterizerStateTrait& trait ) const override;
		virtual SamplerState* CreateSamplerState( const SamplerStateTrait& trait ) const override;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) override;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) override;

		// Canvas
		virtual Canvas* CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format, const float4& clearColor ) const override;

		// Viewport
		virtual Viewport* CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) const override;
		virtual Viewport* CreateViewport( Canvas& canvas ) const override;

		virtual GpuTimer* CreateGpuTimer() const override;
		virtual OcclusionQuery* CreateOcclusionQuery() const override;
		virtual PipelineStatistics* CreatePipelineStatistics() const override;

		virtual void SetPSOCache( std::map<uint64, BinaryChunk>& psoCache ) override;
		virtual void SetPSOCache( const BinaryChunk& psoCache ) override;
		virtual BinaryChunk SerializePSOLibraryCache() override;

		virtual void PostReloadShaders() override;

		virtual BLAS* CreateBLAS( const BLASDesc& desc, const char* debugName ) const override;
		virtual TLAS* CreateTLAS( const TLASDesc& desc, const char* debugName ) const override;

		D3D11ResourceManager() = default;
		virtual ~D3D11ResourceManager() override;
		D3D11ResourceManager( const D3D11ResourceManager& ) = delete;
		D3D11ResourceManager( D3D11ResourceManager&& ) = delete;
		D3D11ResourceManager& operator=( const D3D11ResourceManager& ) = delete;
		D3D11ResourceManager& operator=( D3D11ResourceManager&& ) = delete;

	private:
		std::map<GraphicsPipelineStateInitializer, RefHandle<GraphicsPipelineState>> m_graphicsPipelineStateCache;
		std::map<ComputePipelineStateInitializer, RefHandle<ComputePipelineState>> m_computePipelineStateCache;
	};

	Owner<IResourceManager*> CreateD3D11ResourceManager();
}
