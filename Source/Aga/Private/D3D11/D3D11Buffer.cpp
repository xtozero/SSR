#include "stdafx.h"
#include "D3D11Buffer.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

namespace
{
	D3D11_BUFFER_DESC ConvertTraitToDesc( const BUFFER_TRAIT& trait )
	{
		UINT byteWidth = trait.m_count * trait.m_stride;
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
		UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );
		UINT structureByteStride = trait.m_stride;

		return D3D11_BUFFER_DESC{
			byteWidth,
			usage,
			bindFlag,
			cpuAccessFlag,
			miscFlags,
			structureByteStride
		};
	}
}

void CD3D11Buffer::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateBuffer( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Buffer::CD3D11Buffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitToDesc( trait );
}

D3D11BufferBase::D3D11BufferBase( const BUFFER_TRAIT& trait, const void* initData )
{
	m_desc = ConvertTraitToDesc( trait );
	m_dataStorage = new unsigned char[m_desc.ByteWidth];
	m_initData.pSysMem = m_dataStorage;
	m_initData.SysMemPitch = m_desc.ByteWidth;
	m_initData.SysMemSlicePitch = m_desc.ByteWidth;
	
	if ( initData != nullptr )
	{
		m_hasInitData = true;
		std::memcpy( m_dataStorage, initData, m_desc.ByteWidth );
	}
}

D3D11BufferBase::~D3D11BufferBase( )
{
	delete[] m_dataStorage;
	m_dataStorage = nullptr;

	DestroyBuffer( );
}

void D3D11BufferBase::CreateBuffer( )
{
	D3D11_SUBRESOURCE_DATA* initData = m_hasInitData ? &m_initData : nullptr;
	HRESULT hr = D3D11Device( ).CreateBuffer( &m_desc, initData, &m_buffer );
	assert( SUCCEEDED( hr ) );
}

void D3D11BufferBase::DestroyBuffer( )
{
	if ( m_buffer )
	{
		ULONG ref = m_buffer->Release( );
		m_buffer = nullptr;
		assert( ref == 0 );
	}
}

void D3D11ConstantBuffer::InitResource( )
{
	CreateBuffer( );
}

void D3D11ConstantBuffer::Free( )
{
	DestroyBuffer( );
}

D3D11ConstantBuffer::D3D11ConstantBuffer( const BUFFER_TRAIT& trait ) : D3D11BufferBase( trait, nullptr )
{
	InitResource( );
}

void D3D11VertexBuffer::InitResource( )
{
	CreateBuffer( );
}

void D3D11VertexBuffer::Free( )
{
	DestroyBuffer( );
}

D3D11VertexBuffer::D3D11VertexBuffer( const BUFFER_TRAIT& trait, const void* initData ) : D3D11BufferBase( trait, initData )
{
	InitResource( );
}

void D3D11IndexBuffer::InitResource( )
{
	CreateBuffer( );
}

void D3D11IndexBuffer::Free( )
{
	DestroyBuffer( );
}

D3D11IndexBuffer::D3D11IndexBuffer( const BUFFER_TRAIT& trait, const void* initData ) : D3D11BufferBase( trait, initData )
{
	InitResource( );
}
