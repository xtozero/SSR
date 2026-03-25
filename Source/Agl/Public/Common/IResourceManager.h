#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <map>

namespace agl
{
	class AmplificationShader;
	class BLAS;
	class BlendState;
	class Buffer;
	class Canvas;
	class ComputePipelineState;
	class ComputePipelineStateDesc;
	class ComputeShader;
	class DepthStencilState;
	class GeometryShader;
	class GraphicsPipelineState;
	class GraphicsPipelineStateDesc;
	class MeshShader;
	class OcclusionQuery;
	class PipelineStatistics;
	class PixelShader;
	class GpuTimer;
	class RasterizerState;
	class RaytracingPipelineState;
	class SamplerState;
	class Shader;
	class ShaderParameterInfo;
	class TLAS;
	class Texture;
	class VertexLayout;
	class VertexShader;
	class Viewport;

	struct BLASDesc;
	struct RaytracingPipelineStateDesc;
	struct TLASDesc;

	class IResourceManager
	{
	public:
		virtual void Shutdown() = 0;

		virtual Texture* CreateTexture( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData ) const = 0;

		virtual Buffer* CreateBuffer( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData ) const = 0;

		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size ) const = 0;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const = 0;
		virtual VertexShader* CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const = 0;
		virtual GeometryShader* CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const = 0;
		virtual PixelShader* CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const = 0;
		virtual MeshShader* CreateMeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const = 0;
		virtual AmplificationShader* CreateAmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const = 0;
		virtual RayGenerationShader* CreateRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const = 0;
		virtual IntersectionShader* CreateIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const = 0;
		virtual AnyHitShader* CreateAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const = 0;
		virtual ClosestHitShader* CreateClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const = 0;
		virtual MissShader* CreateMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const = 0;
		virtual CallableShader* CreateCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const = 0;

		virtual BlendState* CreateBlendState( const BlendStateTrait& trait ) const = 0;
		virtual DepthStencilState* CreateDepthStencilState( const DepthStencilStateTrait& trait ) const = 0;
		virtual RasterizerState* CreateRasterizerState( const RasterizerStateTrait& trait ) const = 0;
		virtual SamplerState* CreateSamplerState( const SamplerStateTrait& trait ) const = 0;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateDesc& desc ) = 0;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateDesc& desc ) = 0;

		virtual Canvas* CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format, const float4& clearColor ) const = 0;

		virtual Viewport* CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) const = 0;
		virtual Viewport* CreateViewport( Canvas& canvas ) const = 0;

		virtual GpuTimer* CreateGpuTimer() const = 0;
		virtual OcclusionQuery* CreateOcclusionQuery() const = 0;
		virtual PipelineStatistics* CreatePipelineStatistics() const = 0;

		virtual void SetPSOCache( std::map<uint64, BinaryChunk>& psoCache ) = 0;
		virtual void SetPSOCache( const BinaryChunk& psoCache ) = 0;
		virtual BinaryChunk SerializePSOLibraryCache() = 0;

		virtual void PostReloadShaders() = 0;

		virtual BLAS* CreateBLAS( const BLASDesc& desc, const char* debugName ) const = 0;
		virtual TLAS* CreateTLAS( const TLASDesc& desc, const char* debugName ) const = 0;
		virtual RaytracingPipelineState* CreateRaytracingPipelineState( const RaytracingPipelineStateDesc& desc ) = 0;

		virtual ~IResourceManager() = default;
	};
}
