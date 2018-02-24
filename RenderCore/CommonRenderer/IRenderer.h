#pragma once

#include "../common.h"
#include "../../shared/Util.h"
#include "Resource.h"

#include <memory>

class IMaterial;
class IRenderResource;
class IResourceManager;
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
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void ShutDownRenderer( ) = 0;
	virtual void SceneBegin( ) = 0;
	virtual void ForwardRenderEnd( ) = 0;
	virtual BYTE SceneEnd( ) = 0;

	virtual RE_HANDLE CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual RE_HANDLE CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual RE_HANDLE CreateComputeShader( const TCHAR* pFilePath, const char* pProfile ) = 0;

	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait ) = 0;

	virtual void* LockBuffer( RE_HANDLE buffer, int lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) = 0;
	virtual void UnLockBuffer( RE_HANDLE buffer, UINT subResource = 0 ) = 0;

	virtual RE_HANDLE FindGraphicsShaderByName( const TCHAR* name ) = 0;
	virtual RE_HANDLE FindComputeShaderByName( const TCHAR* pName ) = 0;

	virtual IMaterial* SearchMaterial( const TCHAR* pMaterialName ) = 0;

	virtual void SetViewports( std::vector<Viewport>& viewports ) = 0;
	virtual void SetScissorRects( std::vector<RECT>& rects ) = 0;

	virtual RE_HANDLE CreateShaderResourceFromFile( const String& fileName ) = 0;

	virtual RE_HANDLE CreateRasterizerState( const String& stateName ) = 0;
	virtual RE_HANDLE CreateSamplerState( const String& stateName ) = 0;
	virtual RE_HANDLE CreateDepthStencilState( const String& stateName ) = 0;
	virtual RE_HANDLE CreateBlendState( const String& stateName ) = 0;

	virtual void TakeSnapshot2D( const String& sourceTextureName, const String& destTextureName ) = 0;
	
	virtual void ClearRendertarget( RE_HANDLE renderTarget, const float( &clearColor )[4] ) = 0;
	virtual void ClearDepthStencil( RE_HANDLE depthStencil, float depthColor, UINT8 stencilColor ) = 0;

	virtual void BindVertexBuffer( RE_HANDLE* pVertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets ) = 0;
	virtual void BindIndexBuffer16( RE_HANDLE indexBuffer, UINT indexOffset ) = 0;
	virtual void BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, const RE_HANDLE* pConstantBuffers ) = 0;
	virtual void BindShader( SHADER_TYPE type, RE_HANDLE shader ) = 0;
	virtual void BindShaderResource( SHADER_TYPE type, int startSlot, int count, const RE_HANDLE* resource ) = 0;
	virtual void BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource ) = 0;
	virtual void BindRenderTargets( const RE_HANDLE* pRenderTargets, int renderTargetCount, RE_HANDLE depthStencil ) = 0;
	virtual void BindRasterizerState( RE_HANDLE rasterizerState ) = 0;
	virtual void BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, RE_HANDLE* pSamplerStates ) = 0;
	virtual void BindDepthStencilState( RE_HANDLE depthStencilState ) = 0;
	virtual void BindBlendState( RE_HANDLE blendState ) = 0;

	virtual void Draw( UINT primitive, UINT vertexCount, UINT vertexOffset = 0 ) = 0;
	virtual void DrawIndexed( UINT primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) = 0;
	virtual void DrawInstanced( UINT primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawInstancedInstanced( UINT primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawAuto( UINT primitive ) = 0;
	virtual void Dispatch( int x, int y, int z = 1 ) = 0;

	virtual IResourceManager& GetResourceManager( ) = 0;

	virtual void GenerateMips( RE_HANDLE shaderResource ) = 0;

	virtual ~IRenderer( ) = default;

protected:
	IRenderer( ) = default;
};

