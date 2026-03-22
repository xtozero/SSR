#pragma once

#include "GuideTypes.h"
#include "HashUtil.h"
#include "IResourceManager.h"

#include <d3d12.h>
#include <map>
#include <shared_mutex>
#include <unordered_map>
#include <wrl/client.h>

namespace agl
{
	class D3D12DisposableConstantBufferPool;
	class D3D12HitGroup;
	class D3D12RaytracingPipelineState;
	class D3D12RaytracingShaderTable;

	struct RaytracingPipelineStateDesc;

	class D3D12ResourceManager final : public IResourceManager
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
		virtual RayGenerationShader* CreateRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const override;
		virtual IntersectionShader* CreateIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const override;
		virtual AnyHitShader* CreateAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const override;
		virtual ClosestHitShader* CreateClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const override;
		virtual MissShader* CreateMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const override;
		virtual CallableShader* CreateCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const override;

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
		virtual RaytracingPipelineState* CreateRaytracingPipelineState( const RaytracingPipelineStateDesc& desc ) override;

		D3D12HitGroup* CreateHitGroup( const HitGroupDesc& desc );
		D3D12RaytracingShaderTable* CreateRaytracingShaderTable( const RaytracingShaderTableDesc& desc );

		ID3D12PipelineState* FindOrCreate( const D3D12ComputePipelineState* pipelineState );
		ID3D12PipelineState* FindOrCreate( const D3D12GraphicsPipelineState* pipelineState, const DXGI_FORMAT( &rtvFormats )[8], DXGI_FORMAT dsvFormat );
		ID3D12CommandSignature* FindOrCreate( IndirectCommandType type );

		D3D12DisposableConstantBufferPool& GetDisposableConstantBufferPool();

		void Prepare();

		D3D12ResourceManager() = default;
		virtual ~D3D12ResourceManager() override;
		D3D12ResourceManager( const D3D12ResourceManager& ) = delete;
		D3D12ResourceManager( D3D12ResourceManager&& ) = delete;
		D3D12ResourceManager& operator=( const D3D12ResourceManager& ) = delete;
		D3D12ResourceManager& operator=( D3D12ResourceManager&& ) = delete;

	private:
		void UpdatePSOCache( size_t hash, ID3D12PipelineState* pipelineState );

		std::map<GraphicsPipelineStateInitializer, RefHandle<GraphicsPipelineState>> m_graphicsPipelineStateCache;
		std::map<ComputePipelineStateInitializer, RefHandle<ComputePipelineState>> m_computePipelineStateCache;

		std::shared_mutex m_d3d12PipelineMutex;
		std::map<uint64, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_d3d12PipelineState;

		std::shared_mutex m_d3d12ComputePipelineMutex;
		std::map<uint64, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_d3d12ComputePipelineState;

		std::map<IndirectCommandType, Microsoft::WRL::ComPtr<ID3D12CommandSignature>> m_d3d12IndirectCommandSignature;

		std::map<uint64, BinaryChunk>* m_psoCache = nullptr;

		std::vector<D3D12DisposableConstantBufferPool> m_d3d12DisposbleConstantBufferPool;

		Microsoft::WRL::ComPtr<ID3D12PipelineLibrary1> m_d3d12PipelineLibrary;

		std::shared_mutex m_d3d12HitGroupMutex;
		std::map<uint64, RefHandle<D3D12HitGroup>> m_d3d12HitGroups;

		std::shared_mutex m_d3d12ShaderTableMutex;
		std::map<uint64, RefHandle<D3D12RaytracingShaderTable>> m_d3d12ShaderTables;

		std::shared_mutex m_d3d12RaytracingPipelineMutex;
		std::map<uint64, RefHandle<D3D12RaytracingPipelineState>> m_d3d12RaytracingPipelineStates;
	};

	Owner<IResourceManager*> CreateD3D12ResourceManager();
	uint32 GetIndirectArgumentStride( agl::IndirectCommandType type );
}
