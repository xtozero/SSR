#include "stdafx.h"
#include "D3D11Buffer.h"

#include "D3D11RenderStateManager.h"
#include "D3D11Resource.h"

#include <cassert>
#include <D3D11.h>

D3D11_BUFFER_DESC InitBufferDesc( const BUFFER_TRAIT& trait )
{
	UINT byteWidth = trait.m_count * trait.m_stride;
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_bufferType );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );
	UINT structureByteStride = trait.m_stride;

	return D3D11_BUFFER_DESC{ byteWidth, usage, bindFlag, cpuAccessFlag, miscFlags, structureByteStride };
}

bool CD3D11Buffer::Create( ID3D11Device& device, const BUFFER_TRAIT& trait )
{
	D3D11_BUFFER_DESC desc = InitBufferDesc(trait);

	D3D11_SUBRESOURCE_DATA* pSrd = nullptr;
	D3D11_SUBRESOURCE_DATA srd = {};

	if ( trait.m_srcData )
	{
		pSrd = &srd;

		srd.pSysMem = trait.m_srcData ;
		srd.SysMemPitch = trait.m_pitch;
		srd.SysMemSlicePitch = trait.m_slicePitch;
	}

	m_bufferSize = trait.m_count * trait.m_stride;
	HRESULT hr = device.CreateBuffer( &desc, pSrd, m_buffer.GetAddressOf( ) );

	return SUCCEEDED( hr );
}

void CD3D11Buffer::SetVertexBuffer( const UINT* pStride, const UINT* pOffset ) const
{
	m_renderStateManager.SetVertexBuffer( 0, 1, m_buffer.GetAddressOf( ), pStride, pOffset );
}

void CD3D11Buffer::SetIndexBuffer( UINT stride, UINT offset ) const
{
	m_renderStateManager.SetIndexBuffer( m_buffer.Get( ), sizeof( WORD ) == stride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, offset );
}

void CD3D11Buffer::SetVSBuffer( const UINT startSlot ) const
{
	m_renderStateManager.SetVsConstantBuffers( startSlot, 1, m_buffer.GetAddressOf( ) );
}

void CD3D11Buffer::SetPSBuffer( const UINT startSlot ) const
{
	m_renderStateManager.SetPsConstantBuffers( startSlot, 1, m_buffer.GetAddressOf( ) );
}

void CD3D11Buffer::SetCSBuffer( const UINT startSlot ) const
{
	m_renderStateManager.SetCsConstantBuffers( startSlot, 1, m_buffer.GetAddressOf( ) );
}

void* CD3D11Buffer::LockBuffer( UINT lockFlag, UINT subResource )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT hr = m_renderStateManager.Map( m_buffer.Get( ), subResource, ConvertLockFlagToD3D11Map( lockFlag ), 0, &mappedResource );
	
	if ( SUCCEEDED( hr ) )
	{
		return mappedResource.pData;
	}

	return nullptr;
}

void CD3D11Buffer::UnLockBuffer( UINT subResource )
{
	m_renderStateManager.Unmap( m_buffer.Get( ), subResource );
}

CD3D11Buffer::CD3D11Buffer( CD3D11RenderStateManager& renderStateManager ) : m_renderStateManager( renderStateManager ) {}
