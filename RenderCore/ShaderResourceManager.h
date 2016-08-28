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
	void LoadShaderResourceFromFile( ID3D11Device* pDevice, const String& fileName );
	IShaderResource* FindShaderResource( const String& fileName ) const;
	void RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView, int srcFlag = 0 );
	virtual IShaderResource* CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc, const String& resourceName, int srcFlag = 0 );

	CShaderResourceManager( );
	~CShaderResourceManager( );

private:
	std::map<String, std::shared_ptr<IShaderResource>> m_shaderResources;
};

