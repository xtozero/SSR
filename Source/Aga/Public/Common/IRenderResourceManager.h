#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

namespace aga
{
	class Buffer;
	class ComputeShader;
	class DepthStencilState;
	class PixelShader;
	class Texture;
	class VertexShader;
}

class VertexLayout;
class VertexLayoutDesc;
struct BUFFER_TRAIT;
struct RESOURCE_INIT_DATA;
struct RESOURCE_REGION;
struct TEXTURE_TRAIT;

class IResourceManager
{
public:
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;

	virtual aga::Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual aga::Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) = 0;

	virtual VertexLayout* FindAndCreateVertexLayout( const aga::VertexShader* vs, const VertexLayoutDesc& layoutDesc ) = 0;
	virtual aga::ComputeShader* CreateComputeShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual aga::VertexShader* CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual aga::PixelShader* CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) = 0;

	virtual RE_HANDLE CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateBlendState( const BLEND_STATE_TRAIT& trait ) = 0;
	virtual aga::DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) = 0;

	virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) = 0;
	virtual void UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull = nullptr ) = 0;

	virtual void FreeResource( RE_HANDLE handle ) = 0;

	virtual ~IResourceManager( ) = default;
};