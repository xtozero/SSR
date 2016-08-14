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

enum SOURCE_RESOURCE_FLAG
{
	TEXTURE = 0,
	RENDER_TARGET,
	DEPTH_STENCIL,
};

class IShaderResource
{
public:
	virtual bool LoadShaderResource( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual ID3D11ShaderResourceView* Get( ) const = 0;
	virtual void SetShaderResourceView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView ) = 0;
	virtual bool CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc ) = 0;
	
	virtual bool IsDepthStencil( ) const = 0;
	virtual bool IsRenderTarget( ) const = 0;
	virtual bool IsTexture( ) const = 0;

	virtual ~IShaderResource( ) = default;

protected:
	IShaderResource( ) = default;
};