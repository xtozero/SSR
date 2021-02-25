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

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_BUFFER_DESC& desc )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

		if ( desc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
		{
			if ( ( desc.ByteWidth % 4 ) != 0 )
			{
				__debugbreak( );
			}

			srv.Format = DXGI_FORMAT_R32_TYPELESS;
			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srv.BufferEx.NumElements = desc.ByteWidth / 4;
			srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		}
		else
		{
			srv.Format = DXGI_FORMAT_UNKNOWN;
			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srv.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
		}

		return srv;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_BUFFER_DESC& desc )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		if ( desc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
		{
			if ( ( desc.ByteWidth % 4 ) != 0 )
			{
				__debugbreak( );
			}

			uav.Format = DXGI_FORMAT_R32_TYPELESS;
			uav.Buffer.NumElements = desc.ByteWidth / 4;
			uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		}
		else
		{
			uav.Format = DXGI_FORMAT_UNKNOWN;
			uav.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
			uav.Buffer.Flags = 0; // TODO : handle append / counter flag later
		}

		return uav;
	}
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

	Free( );
}

void D3D11BufferBase::InitResource( )
{
	CreateBuffer( );
}

void D3D11BufferBase::FreeResource( )
{
	DestroyBuffer( );
}

void D3D11BufferBase::CreateBuffer( )
{
	D3D11_SUBRESOURCE_DATA* initData = m_hasInitData ? &m_initData : nullptr;
	HRESULT hr = D3D11Device( ).CreateBuffer( &m_desc, initData, &m_buffer );
	assert( SUCCEEDED( hr ) );

	if ( m_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_desc );
		hr = D3D11Device( ).CreateShaderResourceView( m_buffer, &srvDesc, &m_srv );
		assert( SUCCEEDED( hr ) );
	}

	if ( m_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_desc );
		hr = D3D11Device( ).CreateUnorderedAccessView( m_buffer, &uavDesc, &m_uav );
		assert( SUCCEEDED( hr ) );
	}
}

void D3D11BufferBase::DestroyBuffer( )
{
	if ( m_uav )
	{
		ULONG ref = m_uav->Release( );
		m_uav = nullptr;
		assert( ref == 0 );
	}

	if ( m_srv )
	{
		ULONG ref = m_srv->Release( );
		m_srv = nullptr;
		assert( ref == 0 );
	}

	if ( m_buffer )
	{
		ULONG ref = m_buffer->Release( );
		m_buffer = nullptr;
		assert( ref == 0 );
	}
}
