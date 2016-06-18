#pragma once

#include "common.h"
#include <memory>

class IShader;
class IBuffer;
class IRenderView;
class IMaterial;
class IMesh;
class IShaderResource;
class ISampler;
struct ID3D11RasterizerState;
struct ID3D11SamplerState;
struct ID3D11DepthStencilState;
struct D3DXMATRIX;
enum SHADER_TYPE;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

enum class RENDERTARGET_FLAG
{
	NONE = 0,
	DEFALUT,
};

enum class DEPTHSTENCIL_FLAG
{
	NONE = 0,
	DEFALUT
};

class RENDERCORE_DLL IRenderer
{
public:
	virtual bool InitializeRenderer ( HWND hWind, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void ShutDownRenderer ( ) = 0;
	virtual void ClearRenderTargetView ( ) = 0;
	virtual void ClearRenderTargetView ( float r, float g, float b, float a ) = 0;
	virtual void ClearDepthStencilView ( ) = 0;
	virtual void SceneBegin( ) = 0;
	virtual void SceneEnd( ) = 0;

	virtual std::shared_ptr<IShader> CreateVertexShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual std::shared_ptr<IShader> CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) = 0;

	virtual std::shared_ptr<IBuffer> CreateVertexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) = 0;
	virtual std::shared_ptr<IBuffer> CreateIndexBuffer( const UINT stride, const UINT numOfElement, const void* srcData ) = 0;
	virtual std::shared_ptr<IBuffer> CreateConstantBuffer( const String& bufferName, const UINT stride, const UINT numOfElement, const void* srcData ) = 0;

	virtual void* MapConstantBuffer( const String& bufferName ) = 0;
	virtual void UnMapConstantBuffer( const String& bufferName ) = 0;
	virtual void SetConstantBuffer( const String& bufferName, const UINT slot, const SHADER_TYPE type ) = 0;

	virtual std::shared_ptr<IShader> SearchShaderByName( const TCHAR* name ) = 0;

	virtual std::shared_ptr<IMaterial> GetMaterialPtr( const TCHAR* pMaterialName ) = 0;
	virtual std::shared_ptr<IMesh> GetModelPtr( const TCHAR* pModelName ) = 0;
	virtual void SetModelPtr( const String& modelName, const std::shared_ptr<IMesh>& pModel ) = 0;
	virtual void DrawModel( std::shared_ptr<IMesh> pModel ) = 0;

	virtual void PushViewPort( const float topLeftX, const float topLeftY, const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f ) = 0;
	virtual void PopViewPort( ) = 0;

	virtual IRenderView* GetCurrentRenderView( ) = 0;

	virtual void UpdateWorldMatrix( const D3DXMATRIX& worldMatrix, const D3DXMATRIX& invWorldMatrix ) = 0;

	virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> CreateRenderState( const String& stateName ) = 0;
	virtual IShaderResource* GetShaderResourceFromFile( const String& fileName ) = 0;
	virtual std::shared_ptr<ISampler> CreateSamplerState( const String& stateName ) = 0;

	virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CreateDepthStencilState( const String& stateName ) = 0;

	virtual bool SetRenderTargetDepthStencilView( RENDERTARGET_FLAG rtFlag = RENDERTARGET_FLAG::DEFALUT, DEPTHSTENCIL_FLAG dsFlag = DEPTHSTENCIL_FLAG::DEFALUT ) = 0;

	virtual void ResetResource( const std::shared_ptr<IMesh>& pMesh, const SHADER_TYPE type ) = 0;
protected:
	IRenderer( ) = default;
public:
	virtual ~IRenderer( ) = default;
};

