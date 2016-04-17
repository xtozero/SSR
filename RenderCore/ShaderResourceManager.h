#pragma once

#include "common.h"
#include <map>
#include <memory>

struct D3D11_SHADER_RESOURCE_VIEW_DESC;
class IShaderResource;
struct ID3D11Device;
struct ID3D11ShaderResourceView;
class ITexture;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class CShaderResourceManager
{
public:
	void LoadShaderResourceFromFile( ID3D11Device* pDevice, const String& fileName );
	std::shared_ptr<IShaderResource> FindShaderResource( const String& fileName ) const;
	void RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView, int srcFlag = 0 );
	bool CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc, const String& resourceName, int srcFlag = 0 );

	CShaderResourceManager( );
	~CShaderResourceManager( );

private:
	std::map<String, std::shared_ptr<IShaderResource>> m_shaderResources;
};

