#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

struct BUFFER_TRAIT;
struct RESOURCE_INIT_DATA;
struct RESOURCE_REGION;
struct TEXTURE_TRAIT;
struct VERTEX_LAYOUT_TRAIT;

namespace aga
{
	class BlendState;
	class Buffer;
	class ComputeShader;
	class DepthStencilState;
	class PixelShader;
	class RasterizerState;
	class SamplerState;
	class Texture;
	class VertexLayout;
	class VertexShader;

	class IResourceManager
	{
	public:
		virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;

		virtual Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

		virtual Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) = 0;

		virtual VertexLayout* CreateVertexLayout( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, std::size_t size ) = 0;
		virtual ComputeShader* CreateComputeShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
		virtual VertexShader* CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
		virtual PixelShader* CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) = 0;

		virtual BlendState* CreateBlendState( const BLEND_STATE_TRAIT& trait ) = 0;
		virtual DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) = 0;
		virtual RasterizerState* CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) = 0;
		virtual SamplerState* CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) = 0;
		virtual PipelineState* CreatePipelineState( const PipelineStateInitializer& initializer ) = 0;

		// virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) = 0;
		// virtual void UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull = nullptr ) = 0;

		virtual ~IResourceManager( ) = default;
	};
}