#pragma once

#include "../CommonRenderer/IRenderResource.h"

#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11ShaderResourceView;
struct TEXTURE_TRAIT;

class IBuffer;
class ITexture;

class CD3D11ShaderResource : public IRenderResource
{
public:
	virtual void* Get( ) const override
	{
		return m_pShaderResourceView.Get( );
	}
	
	void SetShaderResourceView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView )
	{
		m_pShaderResourceView = shaderResourceView;
	}
	
	bool LoadShaderResourceFromFile( ID3D11Device& device, const String& fileName );
	bool CreateShaderResource( ID3D11Device& device, const ITexture& texture, const TEXTURE_TRAIT* traitOrNull );
	bool CreateShaderResource( ID3D11Device& device, const IBuffer& buffer, const BUFFER_TRAIT* traitOrNull );

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
};

