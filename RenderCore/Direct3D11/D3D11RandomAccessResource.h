#pragma once
#include "../CommonRenderer/IRenderResource.h"

#include <wrl/client.h>

class IBuffer;
struct ID3D11Device;
struct ID3D11UnorderedAccessView;

class CD3D11RandomAccessResource : public IRenderResource
{
public:
	virtual void* Get( ) const override
	{
		return m_pUnorderedAccessView.Get( );
	}

	void SetRandomAccessResource( Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& randomAccess )
	{
		m_pUnorderedAccessView = randomAccess;
	}

	bool CreateRandomAccessResource( ID3D11Device& device, const ITexture& texture, const TEXTURE_TRAIT* traitOrNull );
	bool CreateRandomAccessResource( ID3D11Device& device, const IBuffer& buffer, const BUFFER_TRAIT* traitOrNull );

private:
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_pUnorderedAccessView;
};

