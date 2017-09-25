#include "stdafx.h"
#include "D3D11Buffer.h"

#include <cassert>
#include <D3D11.h>

inline D3D11_USAGE TranslateUsage( UINT accessFlag )
{
	using namespace BUFFER_ACCESS_FLAG;

	switch ( accessFlag )
	{
	case ( GPU_READ | GPU_WRITE ):
		return D3D11_USAGE_DEFAULT;
		break;
	case ( GPU_READ | CPU_WRITE ):
		return D3D11_USAGE_DYNAMIC;
		break;
	case GPU_READ:
		return D3D11_USAGE_IMMUTABLE;
		break;
	case ( GPU_READ | GPU_WRITE | CPU_READ | CPU_WRITE ):
		return D3D11_USAGE_STAGING;
		break;
	default:
		assert( false );
		break;
	}
}

inline UINT TranslateType( UINT type )
{
	using namespace BUFFER_TYPE;

	UINT ret = 0;

	if ( type & VERTEX_BUFFER )
	{
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}
	
	if ( type & INDEX_BUFFER )
	{
		ret |= D3D11_BIND_INDEX_BUFFER;
	}
	
	if ( type & CONSTANT_BUFFER )
	{
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}
	
	if ( type & SHADER_RESOURCE )
	{
		ret |= D3D11_BIND_SHADER_RESOURCE;
	}
	
	if ( type & STREAM_OUTPUT )
	{
		ret |= D3D11_BIND_STREAM_OUTPUT;
	}
	
	if ( type & RENDER_TARGET )
	{
		ret |= D3D11_BIND_RENDER_TARGET;
	}

	if ( type & DEPTH_STENCIL )
	{
		ret |= D3D11_BIND_DEPTH_STENCIL;
	}

	if ( type & UNORDERED_ACCESS )
	{
		ret |= D3D11_BIND_UNORDERED_ACCESS;
	}

	return ret;
}

inline UINT TranslateMiscFlag( UINT miscFlag )
{
	using namespace BUFFER_MISC;

	UINT ret = 0;

	if ( miscFlag & BUFFER_STRUCTURED )
	{
		ret |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	return ret;
}

inline UINT TranslateCpuFlag( UINT accessFlag )
{
	using namespace BUFFER_ACCESS_FLAG;

	UINT ret = 0;

	if ( accessFlag & CPU_READ )
	{
		ret |= D3D11_CPU_ACCESS_READ;
	}

	if ( accessFlag & CPU_WRITE )
	{
		ret |= D3D11_CPU_ACCESS_WRITE;
	}

	return ret;
}

D3D11_BUFFER_DESC InitBufferDesc( const BUFFER_TRAIT& trait )
{
	UINT byteWidth = trait.m_count * trait.m_stride;
	D3D11_USAGE usage = TranslateUsage( trait.m_access );
	UINT bindFlag = TranslateType( trait.m_bufferType );
	UINT cpuAccessFlag = TranslateCpuFlag( trait.m_access );
	UINT miscFlags = TranslateMiscFlag( trait.m_miscFlag );
	UINT structureByteStride = ( trait.m_miscFlag & BUFFER_MISC::BUFFER_STRUCTURED ) ? trait.m_stride : 0;

	return D3D11_BUFFER_DESC{ byteWidth, usage, bindFlag, cpuAccessFlag, miscFlags, structureByteStride };
}

bool D3D11Buffer::Create( ID3D11Device* pDevice, const BUFFER_TRAIT& trait )
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
	HRESULT hr = pDevice->CreateBuffer( &desc, pSrd, m_buffer.GetAddressOf( ) );

	return SUCCEEDED( hr );
}

void D3D11Buffer::SetVertexBuffer( const UINT* pStride, const UINT* pOffset ) const
{
	m_pDeviceContext->IASetVertexBuffers( 0, 1, m_buffer.GetAddressOf( ), pStride, pOffset );
}

void D3D11Buffer::SetIndexBuffer( UINT stride, UINT offset ) const
{
	m_pDeviceContext->IASetIndexBuffer( m_buffer.Get( ), sizeof( WORD ) == stride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, offset );
}

void D3D11Buffer::SetVSBuffer( const UINT startSlot ) const
{
	m_pDeviceContext->VSSetConstantBuffers( startSlot, 1, m_buffer.GetAddressOf( ) );
}

void D3D11Buffer::SetPSBuffer( const UINT startSlot ) const
{
	m_pDeviceContext->PSSetConstantBuffers( startSlot, 1, m_buffer.GetAddressOf( ) );
}

void* D3D11Buffer::LockBuffer( UINT subResource )
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT hr = m_pDeviceContext->Map( m_buffer.Get( ), subResource, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
	
	if ( SUCCEEDED( hr ) )
	{
		return mappedResource.pData;
	}

	return nullptr;
}

void D3D11Buffer::UnLockBuffer( UINT subResource )
{
	m_pDeviceContext->Unmap( m_buffer.Get( ), subResource );
}

D3D11Buffer::D3D11Buffer( ID3D11DeviceContext* pDeviceContext ) : m_pDeviceContext( pDeviceContext )
{
	assert( m_pDeviceContext );
}
