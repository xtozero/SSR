#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <map>

namespace agl
{
	class BlendState;
	class Buffer;
	class Canvas;
	class ComputePipelineState;
	class ComputePipelineStateInitializer;
	class ComputeShader;
	class DepthStencilState;
	class GeometryShader;
	class GraphicsPipelineState;
	class GraphicsPipelineStateInitializer;
	class OcclusionQuery;
	class PixelShader;
	class GpuTimer;
	class RasterizerState;
	class SamplerState;
	class ShaderParameterInfo;
	class Texture;
	class VertexLayout;
	class VertexShader;
	class Viewport;

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

		virtual BlendState* CreateBlendState( const BlendStateTrait& trait ) const = 0;
		virtual DepthStencilState* CreateDepthStencilState( const DepthStencilStateTrait& trait ) const = 0;
		virtual RasterizerState* CreateRasterizerState( const RasterizerStateTrait& trait ) const = 0;
		virtual SamplerState* CreateSamplerState( const SamplerStateTrait& trait ) const = 0;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) = 0;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) = 0;

		virtual Canvas* CreateCanvas( uint32 width, uint32 height, void* hWnd, ResourceFormat format ) const = 0;

		virtual Viewport* CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) const = 0;
		virtual Viewport* CreateViewport( Canvas& canvas ) const = 0;

		virtual GpuTimer* CreateGpuTimer() const = 0;
		virtual OcclusionQuery* CreateOcclusionQuery() const = 0;

		virtual void SetPSOCache( std::map<uint64, BinaryChunk>& psoCache ) = 0;

		virtual ~IResourceManager() = default;
	};
}
