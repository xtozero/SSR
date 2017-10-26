#pragma once

#include "../common.h"
#include "../../shared/Util.h"

#include <memory>

class IBuffer;
class IMaterial;
class IRenderResource;
class IRenderState;
class IRenderView;
class IResourceManager;
class IShader;
class RenderTargetBinder;
struct BUFFER_TRAIT;
struct CXMFLOAT4X4;
struct IDXGISwapChain;
struct Viewport;

enum SHADER_TYPE;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IRenderer
{
public:
	virtual bool InitializeRenderer( HWND hWind, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void ShutDownRenderer( ) = 0;
	virtual void SceneBegin( ) = 0;
	virtual void ForwardRenderEnd( ) = 0;
	virtual void SceneEnd( ) = 0;

	virtual IShader* CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual IShader* CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) = 0;

	virtual IBuffer* CreateBuffer( const BUFFER_TRAIT& trait ) = 0;

	virtual IShader* SearchShaderByName( const TCHAR* name ) = 0;

	virtual IMaterial* GetMaterialPtr( const TCHAR* pMaterialName ) = 0;

	virtual void SetViewports( std::vector<Viewport>& viewports ) = 0;
	virtual void SetScissorRects( std::vector<RECT>& rects ) = 0;

	virtual IRenderState* CreateRenderState( const String& stateName ) = 0;
	virtual IRenderResource* GetShaderResourceFromFile( const String& fileName ) = 0;
	virtual IRenderState* CreateSamplerState( const String& stateName ) = 0;

	virtual IRenderState* CreateDepthStencilState( const String& stateName ) = 0;

	virtual IRenderState* CreateBlendState( const String& stateName ) = 0;

	virtual void TakeSnapshot2D( const String& sourceTextureName, const String& destTextureName ) = 0;
	
	virtual void PSSetShaderResource( UINT startSlot, IRenderResource* shaderResourceOrNull ) = 0;

	virtual void ClearRendertarget( IRenderResource& renderTarget, const float( &clearColor )[4] ) = 0;
	virtual void ClearDepthStencil( IRenderResource& depthStencil, float depthColor, UINT8 stencilColor ) = 0;

	virtual void SetRenderTarget( IRenderResource* pRenderTarget, IRenderResource* pDepthStencil ) = 0;
	virtual void SetRenderTarget( RenderTargetBinder& binder, IRenderResource* pDepthStencil ) = 0;

	virtual void Draw( UINT primitive, UINT vertexCount, UINT vertexOffset = 0 ) = 0;
	virtual void DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) = 0;
	virtual void DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawAuto( UINT primitive ) = 0;

	virtual IDXGISwapChain* GetSwapChain( ) const = 0;
	virtual IResourceManager& GetResourceManager( ) = 0;

protected:
	IRenderer( ) = default;

public:
	virtual ~IRenderer( ) = default;
};

