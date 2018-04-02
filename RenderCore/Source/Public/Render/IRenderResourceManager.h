#pragma once

#include "common.h"
#include "Resource.h"

struct BUFFER_TRAIT;
struct RESOURCE_INIT_DATA;
struct RESOURCE_REGION;
struct TEXTURE_TRAIT;

class IRenderResource;
class ITexture;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IResourceManager
{
public:
	virtual RE_HANDLE LoadTextureFromFile( const String& fileName ) = 0;

	virtual RE_HANDLE CreateTexture1D( TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture1D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual RE_HANDLE CreateTexture2D( TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;
	virtual RE_HANDLE CreateTexture2D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual RE_HANDLE RegisterTexture2D( const String& textureName, void* pTexture, bool isAppSizeDependent = false ) = 0;
	virtual RE_HANDLE FindTexture( const String& textureName ) const = 0;
	virtual const TEXTURE_TRAIT& GetTextureTrait( RE_HANDLE texhandle ) const = 0;

	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;

	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait ) = 0;

	virtual RE_HANDLE CreateVertexShader( const TCHAR* pFilePath, const char* pProfile, const VERTEX_LAYOUT* layoutOrNull = nullptr, int layoutSize = 0 ) = 0;
	virtual RE_HANDLE CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual RE_HANDLE CreateComputeShader( const TCHAR* pFilePath, const char* pProfile ) = 0;
	virtual RE_HANDLE FindGraphicsShaderByName( const TCHAR* pName ) = 0;
	virtual RE_HANDLE FindComputeShaderByName( const TCHAR* pName ) = 0;

	virtual RE_HANDLE CreateRenderTarget( RE_HANDLE texhandle, const String& renderTargetName, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateDepthStencil( RE_HANDLE texhandle, const String& depthStencilName, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE FindRenderTarget( const String& renderTargetName ) const = 0;
	virtual RE_HANDLE FindDepthStencil( const String& depthStencilName ) const = 0;

	virtual RE_HANDLE CreateShaderResourceFromFile( const String& fileName ) = 0;
	virtual RE_HANDLE FindShaderResource( const String& resourceName ) const = 0;
	virtual RE_HANDLE CreateTextureShaderResource( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateBufferShaderResource( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE FindRandomAccessResource( const String& resourceName ) const = 0;
	virtual RE_HANDLE CreateTextureRandomAccess( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait = nullptr ) = 0;
	virtual RE_HANDLE CreateBufferRandomAccess( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait = nullptr ) = 0;

	virtual RE_HANDLE CreateSamplerState( const String& stateName ) = 0;
	virtual RE_HANDLE CreateRasterizerState( const String& stateName ) = 0;
	virtual RE_HANDLE CreateBlendState( const String& stateName ) = 0;
	virtual RE_HANDLE CreateDepthStencilState( const String& stateName ) = 0;

	virtual RE_HANDLE CreateCloneTexture( RE_HANDLE texHandle, const String& textureName ) = 0;

	virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) = 0;
};