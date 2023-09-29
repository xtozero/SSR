#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace agl
{
	class BlendState;
	class Buffer;
	class ComputePipelineState;
	class ComputePipelineStateInitializer;
	class ComputeShader;
	class DepthStencilState;
	class GeometryShader;
	class GraphicsPipelineState;
	class GraphicsPipelineStateInitializer;
	class PixelShader;
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

		virtual Texture* CreateTexture( const TextureTrait& trait, const ResourceInitData* initData = nullptr ) = 0;

		virtual Buffer* CreateBuffer( const BufferTrait& trait, const void* initData = nullptr ) = 0;

		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size ) = 0;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;
		virtual VertexShader* CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;
		virtual GeometryShader* CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;
		virtual PixelShader* CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;

		virtual BlendState* CreateBlendState( const BlendStateTrait& trait ) = 0;
		virtual DepthStencilState* CreateDepthStencilState( const DepthStencilStateTrait& trait ) = 0;
		virtual RasterizerState* CreateRasterizerState( const RasterizerStateTrait& trait ) = 0;
		virtual SamplerState* CreateSamplerState( const SamplerStateTrait& trait ) = 0;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) = 0;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) = 0;

		virtual Viewport* CreateViewport( uint32 width, uint32 height, void* hWnd, ResourceFormat format, const float4& bgColor, bool useDedicateTexture ) = 0;

		virtual ~IResourceManager() = default;
	};
}
