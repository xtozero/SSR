#include "stdafx.h"
#include "D3D11ComputeShader.h"

#include "../CommonRenderer/IBuffer.h"
#include "../CommonRenderer/IRenderResource.h"

#include <D3D11.h>

void D3D11ComputeShader::SetShader( )
{
	m_deviceContext.CSSetShader( m_pComputeShader.Get( ), nullptr, 0 );
}

void D3D11ComputeShader::SetShaderResource( UINT slot, const IRenderResource* pResource )
{
	ID3D11ShaderResourceView* srv = nullptr;

	if ( pResource && pResource->Get( ) )
	{
		srv = static_cast<ID3D11ShaderResourceView*>( pResource->Get( ) );
	}

	m_deviceContext.CSSetShaderResources( slot, 1, &srv );
}

void D3D11ComputeShader::SetRandomAccessResource( UINT slot, const IRenderResource* pResource )
{
	ID3D11UnorderedAccessView* uav = nullptr;

	if ( pResource && pResource->Get( ) )
	{
		uav = static_cast<ID3D11UnorderedAccessView*>( pResource->Get( ) );
	}

	m_deviceContext.CSSetUnorderedAccessViews( slot, 1, &uav, nullptr );
}

void D3D11ComputeShader::Dispatch( UINT x, UINT y, UINT z )
{
	m_deviceContext.Dispatch( x, y, z );
}

bool D3D11ComputeShader::CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize )
{
	bool result = SUCCEEDED( device.CreateComputeShader(
		byteCodePtr, 
		byteCodeSize, 
		nullptr, 
		&m_pComputeShader ) );

	return result;
}
