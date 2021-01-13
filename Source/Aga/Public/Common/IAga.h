#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "Util.h"

#include <memory>

namespace aga
{
	class PixelShader;
	class VertexShader;
}

class ConstantBuffer;
class IndexBuffer;
class IResourceManager;
class ShaderParameterInfo;
class VertexBuffer;
class VertexLayout;
class VertexLayoutDesc;
struct BUFFER_TRAIT;
struct CXMFLOAT4X4;
struct IDXGISwapChain;
struct Viewport;

class IAga
{
public:
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void WaitGPU( ) = 0;

	virtual RE_HANDLE CreateTexture1D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture2D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture3D( TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait ) = 0;
	virtual ConstantBuffer* CreateConstantBuffer( const BUFFER_TRAIT& trait ) = 0;
	virtual VertexBuffer* CreateVertexBuffer( const BUFFER_TRAIT& trait, const void* initData ) = 0;
	virtual IndexBuffer* CreateIndexBuffer( const BUFFER_TRAIT& trait, const void* initData ) = 0;

	virtual VertexLayout* FindAndCreateVertexLayout( aga::VertexShader* vs, const VertexLayoutDesc& layoutDesc ) = 0;
	//virtual RE_HANDLE CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual RE_HANDLE CreateGeometryShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	//virtual RE_HANDLE CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual RE_HANDLE CreateComputeShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual aga::VertexShader* CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual aga::PixelShader* CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual const ShaderParameterInfo& GetShaderParameterInfo( RE_HANDLE shader ) const = 0;

	virtual RE_HANDLE CreateRenderTarget( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateDepthStencil( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE CreateTexture1DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture2DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture3DShaderResource( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateBufferShaderResource( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE CreateTexture1DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture2DRandomAccess( RE_HANDLE texHandle, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateBufferRandomAccess( RE_HANDLE bufHandle, const BUFFER_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) = 0;
	virtual RE_HANDLE CreateBlendState( const BLEND_STATE_TRAIT& trait ) = 0;

	virtual RE_HANDLE GetBackBuffer( UINT buffer ) const = 0;

	virtual void* LockBuffer( RE_HANDLE buffer, int lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) = 0;
	virtual void UnLockBuffer( RE_HANDLE buffer, UINT subResource = 0 ) = 0;

	virtual void SetViewports( const Viewport* viewPorts, int count ) = 0;
	virtual void SetScissorRects( const RECT* rects, int size ) = 0;

	virtual void ClearRendertarget( RE_HANDLE renderTarget, const float( &clearColor )[4] ) = 0;
	virtual void ClearDepthStencil( RE_HANDLE depthStencil, float depthColor, UINT8 stencilColor ) = 0;

	virtual void BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets ) = 0;
	virtual void BindIndexBuffer( RE_HANDLE indexBuffer, UINT indexOffset ) = 0;
	virtual void BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, const RE_HANDLE* pConstantBuffers ) = 0;
	virtual void BindVertexLayout( RE_HANDLE layout ) = 0;
	virtual void BindShader( RE_HANDLE shader ) = 0;
	virtual void BindShaderResource( SHADER_TYPE type, int startSlot, int count, const RE_HANDLE* resource ) = 0;
	virtual void BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource ) = 0;
	virtual void BindRenderTargets( const RE_HANDLE* pRenderTargets, int renderTargetCount, RE_HANDLE depthStencil ) = 0;
	virtual void BindRasterizerState( RE_HANDLE rasterizerState ) = 0;
	virtual void BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, const RE_HANDLE* pSamplerStates ) = 0;
	virtual void BindDepthStencilState( RE_HANDLE depthStencilState ) = 0;
	virtual void BindBlendState( RE_HANDLE blendState ) = 0;

	virtual void Draw( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT vertexOffset = 0 ) = 0;
	virtual void DrawIndexed( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) = 0;
	virtual void DrawInstanced( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawInstancedInstanced( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawAuto( ) = 0;
	virtual void Dispatch( int x, int y, int z = 1 ) = 0;

	virtual BYTE Present( ) = 0;

	virtual void GenerateMips( RE_HANDLE shaderResource ) = 0;

	virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) = 0;

	virtual ~IAga( ) = default;

protected:
	IAga( ) = default;
};

void CreateAbstractGraphicsApi( );
void DestoryAbstractGraphicsApi( );
