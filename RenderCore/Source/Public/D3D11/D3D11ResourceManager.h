#pragma once

#include "D3D11RenderStateFactory.h"
#include "Render/IRenderResourceManager.h"

#include <map>
#include <memory>
#include <vector>

class KeyValueGroup;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Resource;
struct ID3D11ShaderResourceView;

class CDepthStencil;
class CD3D11BlendState;
class CD3D11Buffer;
class CD3D11DepthStencilState;
class CD3D11RandomAccessResource;
class CD3D11SamplerState;
class CD3D11Texture;
class CRenderTarget;
class CD3D11ShaderResource;
class CD3D11ComputeShader;
class CD3D11GeometryShader;
class CD3D11PixelShader;
class CD3D11VertexShader;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class CD3D11ResourceManager : public IResourceManager
{
public:
	bool Bootup( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );

	// Texture
	virtual RE_HANDLE LoadTextureFromFile( const String& fileName ) override;

	virtual RE_HANDLE CreateTexture1D( TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual RE_HANDLE CreateTexture1D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	virtual RE_HANDLE CreateTexture2D( TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;
	virtual RE_HANDLE CreateTexture2D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData = nullptr ) override;

	virtual RE_HANDLE RegisterTexture2D( const String& textureName, void* pTexture, bool isAppSizeDependent = false ) override;
	virtual RE_HANDLE FindTexture( const String& textureName ) const override;
	virtual const TEXTURE_TRAIT& GetTextureTrait( RE_HANDLE texhandle ) const override;

	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;

	// Buffer
	virtual RE_HANDLE CreateBuffer( const BUFFER_TRAIT& trait ) override;

	// Shader
	virtual RE_HANDLE CreateVertexShader( const TCHAR* pFilePath, const char* pProfile, const VERTEX_LAYOUT* layoutOrNull = nullptr, int layoutSize = 0 ) override;
	virtual RE_HANDLE CreateGeometryShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual RE_HANDLE CreatePixelShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual RE_HANDLE CreateComputeShader( const TCHAR* pFilePath, const char* pProfile ) override;
	virtual RE_HANDLE FindGraphicsShaderByName( const TCHAR* pName ) override;
	virtual RE_HANDLE FindComputeShaderByName( const TCHAR* pName ) override;

	// RenderTarget
	virtual RE_HANDLE CreateRenderTarget( RE_HANDLE texhandle, const String& renderTargetName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateDepthStencil( RE_HANDLE texhandle, const String& depthStencilName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE FindRenderTarget( const String& renderTargetName ) const override;
	virtual RE_HANDLE FindDepthStencil( const String& depthStencilName ) const override;

	// ShaderResource
	virtual RE_HANDLE CreateShaderResourceFromFile( const String& fileName ) override;
	virtual RE_HANDLE FindShaderResource( const String& fileName ) const override;
	virtual RE_HANDLE CreateTextureShaderResource( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateBufferShaderResource( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait = nullptr ) override;

	void RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView );

	// RandomAccessResource
	virtual RE_HANDLE FindRandomAccessResource( const String& resourceName ) const override;
	virtual RE_HANDLE CreateTextureRandomAccess( RE_HANDLE texHandle, const String& resourceName, const TEXTURE_TRAIT* trait = nullptr ) override;
	virtual RE_HANDLE CreateBufferRandomAccess( RE_HANDLE bufHandle, const String& resourceName, const BUFFER_TRAIT* trait = nullptr ) override;

	// RenderState
	virtual RE_HANDLE CreateSamplerState( const String& stateName ) override;
	virtual RE_HANDLE CreateRasterizerState( const String& stateName ) override;
	virtual RE_HANDLE CreateBlendState( const String& stateName ) override;
	virtual RE_HANDLE CreateDepthStencilState( const String& stateName ) override;

	// UTIL
	virtual RE_HANDLE CreateCloneTexture( RE_HANDLE texHandle, const String& textureName ) override;
	virtual void CopyResource( RE_HANDLE dest, const RESOURCE_REGION* destRegionOrNull, RE_HANDLE src, const RESOURCE_REGION* srcRegionOrNull ) override;
	virtual void FreeResource( RE_HANDLE resourceHandle ) override;

	void TakeSnapshot( const String& sourceTextureName, const String& destTextureName );
	void OnDeviceLost( );
	void OnDeviceRestore( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );

	const CDepthStencil& GetDepthstencil( RE_HANDLE handle ) const;
	const CRenderTarget& GetRendertarget( RE_HANDLE handle ) const;
	const CD3D11ShaderResource& GetShaderResource( RE_HANDLE handle ) const;
	const CD3D11RandomAccessResource& GetRandomAccess( RE_HANDLE handle ) const;
	const CD3D11Texture& GetTexture( RE_HANDLE handle ) const;
	const CD3D11Buffer& GetBuffer( RE_HANDLE handle ) const;
	const CD3D11VertexShader& GetVertexShader( RE_HANDLE handle ) const;
	const CD3D11GeometryShader& GetGeometryShader( RE_HANDLE handle ) const;
	const CD3D11PixelShader& GetPixelShader( RE_HANDLE handle ) const;
	const CD3D11ComputeShader& GetComputeShader( RE_HANDLE handle ) const;
	const CD3D11SamplerState& GetSamplerState( RE_HANDLE handle ) const;
	const CD3D11RasterizerState& GetRasterizerState( RE_HANDLE handle ) const;
	const CD3D11BlendState& GetBlendState( RE_HANDLE handle ) const;
	const CD3D11DepthStencilState& GetDepthStencilState( RE_HANDLE handle ) const;

	ID3D11Resource* GetD3D11ResourceGeneric( RE_HANDLE handle ) const;

	CD3D11ResourceManager( ) = default;
	~CD3D11ResourceManager( ) = default;
	CD3D11ResourceManager( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager( CD3D11ResourceManager&& ) = delete;
	CD3D11ResourceManager& operator=( const CD3D11ResourceManager& ) = delete;
	CD3D11ResourceManager& operator=( CD3D11ResourceManager&& ) = delete;

private:
	bool LoadShader( );
	bool CreateVertexShaderFromScript( const KeyValue* desc, const char* profile );

	std::vector<CDepthStencil> m_depthStencils;
	std::vector<CRenderTarget> m_renderTargets;
	std::vector<CD3D11ShaderResource> m_shaderResources;
	std::vector<CD3D11RandomAccessResource> m_randomAccessResource;
	std::vector<CD3D11Texture> m_textures;
	std::vector<CD3D11Buffer> m_buffers;
	std::vector<CD3D11VertexShader> m_vertexShaders;
	std::vector<CD3D11GeometryShader> m_geometryShaders;
	std::vector<CD3D11PixelShader> m_pixelShaders;
	std::vector<CD3D11ComputeShader> m_computeShaders;
	std::vector<CD3D11SamplerState> m_samplerStates;
	std::vector<CD3D11RasterizerState> m_rasterizerStates;
	std::vector<CD3D11BlendState> m_blendStates;
	std::vector<CD3D11DepthStencilState> m_depthStencilStates;

	BYTE* m_freeDepthStencil = nullptr;
	BYTE* m_freeRenderTarget = nullptr;
	BYTE* m_freeShaderResource = nullptr;
	BYTE* m_freeRandomAccess = nullptr;
	BYTE* m_freeTexture = nullptr;
	BYTE* m_freeBuffer = nullptr;

	std::map<String, RE_HANDLE> m_dsvLUT;
	std::map<String, RE_HANDLE> m_rtvLUT;
	std::map<String, RE_HANDLE> m_srvLUT;
	std::map<String, RE_HANDLE> m_ravLUT;
	std::map<String, RE_HANDLE> m_texLUT;
	std::map<String, RE_HANDLE>	m_vertexShaderLUT;
	std::map<String, RE_HANDLE> m_geometryShaderLUT;
	std::map<String, RE_HANDLE>	m_pixelShaderLUT;
	std::map<String, RE_HANDLE> m_computeShaderLUT;
	std::map<String, RE_HANDLE> m_samplerStateLUT;
	std::map<String, RE_HANDLE> m_rasterizerStateLUT;
	std::map<String, RE_HANDLE> m_blendStateLUT;
	std::map<String, RE_HANDLE> m_depthStencilStateLUT;

	std::map<String, TEXTURE_TRAIT> m_textureTraits;

	CD3D11SamplerStateFactory m_SamplerFactory;
	CD3D11RasterizerStateFactory m_RasterizerFactory;
	CD3D11BlendStateFactory m_BlendFactory;
	CD3D11DepthStencilStateFactory m_DepthStencilFactory;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	std::pair<int, int>	m_frameBufferSize = { 0, 0 };
};

