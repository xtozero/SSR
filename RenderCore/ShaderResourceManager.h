#pragma once

#include "common.h"
#include "IRenderResourceManager.h"

#include <map>
#include <memory>

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class CShaderResourceManager : public IShaderResourceManager
{
public:
	virtual void LoadShaderResourceFromFile( ID3D11Device* pDevice, const String& fileName ) override;
	virtual IShaderResource* FindShaderResource( const String& fileName ) const override;
	virtual void RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView ) override;
	virtual IShaderResource* CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc, const String& resourceName ) override;

	CShaderResourceManager( );
	~CShaderResourceManager( );

private:
	std::map<String, std::unique_ptr<IShaderResource>> m_shaderResources;
};

