#include "stdafx.h"
#include "D3D11PixelShader.h"

#include "../common.h"
#include "../CommonRenderer/IBuffer.h"
#include "../CommonRenderer/IRenderResource.h"

#include <D3D11.h>

void D3D11PixelShader::SetShader( )
{
	m_deviceContext.PSSetShader( m_pPixelShader.Get( ), nullptr, 0 );
}

void D3D11PixelShader::SetShaderResource( UINT slot, const IRenderResource* pResource )
{
	ID3D11ShaderResourceView* srv = static_cast<ID3D11ShaderResourceView*>( pResource ? pResource->Get( ) : nullptr );
	m_deviceContext.PSSetShaderResources( slot, 1, &srv );
}

bool D3D11PixelShader::CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize )
{
	bool result = SUCCEEDED( device.CreatePixelShader(
		byteCodePtr,
		byteCodeSize,
		nullptr,
		&m_pPixelShader ) );

	return result;
}

D3D11PixelShader::D3D11PixelShader( ID3D11DeviceContext& deviceContext ) :
m_deviceContext( deviceContext )
{
}
