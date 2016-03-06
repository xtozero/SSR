#include "stdafx.h"
#include "ISampler.h"

#include "IMaterial.h"

#include <D3D11.h>
#include <wrl/client.h>

class CSampler : public ISampler
{
public:
	virtual void SetSampler( ID3D11DeviceContext* pDeviceContext, SHADER_TYPE type ) override;

	explicit CSampler( const Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState );
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};

void CSampler::SetSampler( ID3D11DeviceContext* pDeviceContext, SHADER_TYPE type )
{
	if ( pDeviceContext == nullptr )
	{
		return;
	}

	switch ( type )
	{
	case VS:
		pDeviceContext->VSSetSamplers( 0, 1, m_pSampler.GetAddressOf() );
		break;
	case HS:
		pDeviceContext->HSSetSamplers( 0, 1, m_pSampler.GetAddressOf( ) );
		break;
	case DS:
		pDeviceContext->DSSetSamplers( 0, 1, m_pSampler.GetAddressOf( ) );
		break;
	case GS:
		pDeviceContext->GSSetSamplers( 0, 1, m_pSampler.GetAddressOf( ) );
		break;
	case PS:
		pDeviceContext->PSSetSamplers( 0, 1, m_pSampler.GetAddressOf( ) );
		break;
	case CS:
		pDeviceContext->CSSetSamplers( 0, 1, m_pSampler.GetAddressOf( ) );
		break;
	default:
		break;
	}
}

CSampler::CSampler( const Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState ) :
m_pSampler( samplerState )
{}

std::shared_ptr<ISampler> CreateSampler( const Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState )
{
	return std::make_shared<CSampler>( samplerState );
}