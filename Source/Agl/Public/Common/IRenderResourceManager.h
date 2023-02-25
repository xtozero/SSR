#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

struct BUFFER_TRAIT;
struct RESOURCE_INIT_DATA;
struct RESOURCE_REGION;
struct TEXTURE_TRAIT;
struct VERTEX_LAYOUT_TRAIT;

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

		virtual Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

		virtual Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) = 0;

		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size ) = 0;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;
		virtual VertexShader* CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;
		virtual GeometryShader* CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;
		virtual PixelShader* CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) = 0;

		virtual BlendState* CreateBlendState( const BLEND_STATE_TRAIT& trait ) = 0;
		virtual DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) = 0;
		virtual RasterizerState* CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) = 0;
		virtual SamplerState* CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) = 0;
		virtual GraphicsPipelineState* CreatePipelineState( const GraphicsPipelineStateInitializer& initializer ) = 0;
		virtual ComputePipelineState* CreatePipelineState( const ComputePipelineStateInitializer& initializer ) = 0;

		virtual Viewport* CreateViewport( uint32 width, uint32 height, void* hWnd, ResourceFormat format ) = 0;

		// virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) = 0;
		// virtual void UpdateResourceFromMemory( RE_HANDLE dest, void* src, uint32 srcRowPitch, uint32 srcDepthPitch, const RESOURCE_REGION* destRegionOrNull = nullptr ) = 0;

		virtual ~IResourceManager() = default;
	};
}
