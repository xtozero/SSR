#pragma once

#include "common.h"

#include <memory>

class CShaderResourceViewDescription;
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

namespace SOURCE_RESOURCE_FLAG
{
	enum TYPE
	{
		TEXTURE = 0,
		RENDER_TARGET,
		DEPTH_STENCIL,
	};
}

class IShaderResource
{
public:
	virtual bool LoadShaderResource( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual ID3D11ShaderResourceView* Get( ) const = 0;
	virtual void SetShaderResourceView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView ) = 0;
	virtual bool CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc ) = 0;

	virtual ~IShaderResource( ) = default;

protected:
	IShaderResource( ) = default;
};