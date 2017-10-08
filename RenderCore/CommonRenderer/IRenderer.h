#pragma once

#include "../common.h"
#include "../../shared/Util.h"

#include <memory>

class IBuffer;
class IMaterial;
class IMesh;
class IMeshBuilder;
class IRenderResource;
class IRenderState;
class IRenderView;
class IResourceManager;
class ISampler;
class IShader;
class RenderTargetBinder;
struct BUFFER_TRAIT;
struct CXMFLOAT4X4;
struct IDXGISwapChain;

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
	virtual IMesh* GetModelPtr( const TCHAR* pModelName ) = 0;
	virtual void SetModelPtr( const String& modelName, std::unique_ptr<IMesh> pModel ) = 0;

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) = 0;
	virtual void PopViewPort( ) = 0;
	virtual void PushScissorRect( const RECT& rect ) = 0;
	virtual void PopScissorRect( ) = 0;

	virtual IRenderView* GetCurrentRenderView( ) = 0;

	virtual void UpdateWorldMatrix( const CXMFLOAT4X4& worldMatrix, const CXMFLOAT4X4& invWorldMatrix ) = 0;

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
	virtual IMeshBuilder& GetMeshBuilder( ) = 0;
	virtual IResourceManager& GetResourceManager( ) = 0;

protected:
	IRenderer( ) = default;

public:
	virtual ~IRenderer( ) = default;
};

