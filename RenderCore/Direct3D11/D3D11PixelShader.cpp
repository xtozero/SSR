#include "stdafx.h"
#include "D3D11PixelShader.h"

#include "../common.h"
#include "../IBuffer.h"
#include "../IRenderResource.h"

#include <D3D11.h>

void D3D11PixelShader::SetShader( )
{
	m_pDeviceContext->PSSetShader( m_pPixelShader.Get( ), nullptr, 0 );
}

void D3D11PixelShader::SetShaderResource( UINT slot, const IRenderResource* pResource )
{
	ID3D11ShaderResourceView* srv = static_cast<ID3D11ShaderResourceView*>( pResource ? pResource->Get( ) : nullptr );
	m_pDeviceContext->PSSetShaderResources( slot, 1, &srv );
}

void D3D11PixelShader::SetConstantBuffer( UINT slot, const IBuffer* pBuffer )
{
	if ( pBuffer )
	{
		pBuffer->SetPSBuffer( slot );
	}
}

D3D11PixelShader::D3D11PixelShader( ID3D11DeviceContext* pDeviceContext ) :
m_pDeviceContext( pDeviceContext )
{
}

bool D3D11PixelShader::CreateShaderInternal( ID3D11Device* pDevice, const void * byteCodePtr, const size_t byteCodeSize )
{
	bool result = SUCCEEDED( pDevice->CreatePixelShader(
		byteCodePtr,
		byteCodeSize,
		nullptr,
		&m_pPixelShader ) );

	return result;
}
