#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

class ConstantBuffer;
class IndexBuffer;
class VertexBuffer;
struct BUFFER_TRAIT;
struct RESOURCE_INIT_DATA;
struct RESOURCE_REGION;
struct TEXTURE_TRAIT;

class IResourceManager
{
public:
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;

	virtual RE_HANDLE CreateTexture1D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture2D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture3D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual ConstantBuffer* CreateConstantBuffer( const BUFFER_TRAIT& trait ) = 0;
	virtual VertexBuffer* CreateVertexBuffer( const BUFFER_TRAIT& trait, const void* initData ) = 0;
	virtual IndexBuffer* CreateIndexBuffer( const BUFFER_TRAIT& trait, const void* initData ) = 0;

	virtual RE_HANDLE CreateVertexLayout( RE_HANDLE vsHandle, const VERTEX_LAYOUT* layoutOrNull, int layoutSize ) = 0;
	virtual RE_HANDLE CreateVertexShader( const void* byteCodePtr, std::size_t byteCodeSize ) = 0;
	virtual RE_HANDLE CreateGeometryShader( const void* byteCodePtr, std::size_t byteCodeSize ) = 0;
	virtual RE_HANDLE CreatePixelShader( const void* byteCodePtr, std::size_t byteCodeSize ) = 0;
	virtual RE_HANDLE CreateComputeShader( const void* byteCodePtr, std::size_t byteCodeSize ) = 0;

	virtual RE_HANDLE CreateRenderTarget( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateDepthStencil( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE CreateTexture1DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture2DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture3DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateBufferShaderResource( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE CreateTexture1DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture2DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateBufferRandomAccess( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateBlendState( const BLEND_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) = 0;

	virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) = 0;
	virtual void UpdateResourceFromMemory( RE_HANDLE dest, void* src, UINT srcRowPitch, UINT srcDepthPitch, const RESOURCE_REGION* destRegionOrNull = nullptr ) = 0;

	virtual void FreeResource( RE_HANDLE handle ) = 0;

	virtual ~IResourceManager( ) = default;
};