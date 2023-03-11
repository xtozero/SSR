#include "stdafx.h"
#include "D3D11Buffer.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"
#include "D3D11ResourceViews.h"

using ::agl::BUFFER_TRAIT;
using ::agl::ConvertAccessFlagToCpuFlag;
using ::agl::ConvertAccessFlagToUsage;
using ::agl::ConvertMicsToDXMisc;
using ::agl::ConvertTypeToBind;
using ::agl::ResourceMisc;

namespace
{
	D3D11_BUFFER_DESC ConvertTraitToDesc( const BUFFER_TRAIT& trait )
	{
		uint32 byteWidth = trait.m_count * trait.m_stride;
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		uint32 bindFlag = ConvertTypeToBind( trait.m_bindType );
		uint32 cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );
		uint32 structureByteStride = trait.m_stride;

		if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Intermediate ) )
		{
			bindFlag |= D3D11_BIND_SHADER_RESOURCE;
		}

		return D3D11_BUFFER_DESC{
			byteWidth,
			usage,
			bindFlag,
			cpuAccessFlag,
			miscFlags,
			structureByteStride
		};
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_BUFFER_DESC& desc, DXGI_FORMAT format )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

		if ( ( desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS ) > 0 )
		{
			if ( ( desc.ByteWidth % 4 ) != 0 )
			{
				__debugbreak();
			}

			srv.Format = DXGI_FORMAT_R32_TYPELESS;
			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srv.BufferEx.NumElements = desc.ByteWidth / 4;
			srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		}
		else
		{
			srv.Format = format;
			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srv.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
		}

		return srv;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_BUFFER_DESC& desc, DXGI_FORMAT format )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		if ( ( desc.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS ) > 0 )
		{
			if ( ( desc.ByteWidth % 4 ) != 0 )
			{
				__debugbreak();
			}

			uav.Format = DXGI_FORMAT_R32_TYPELESS;
			uav.Buffer.NumElements = desc.ByteWidth / 4;
			uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		}
		else
		{
			uav.Format = format;
			uav.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
			uav.Buffer.Flags = 0; // TODO : handle append / counter flag later
		}

		return uav;
	}
}

namespace agl
{
	ID3D11Buffer* D3D11Buffer::Resource()
	{
		return m_buffer;
	}

	void* D3D11Buffer::Resource() const
	{
		return m_buffer;
	}

	uint32 D3D11Buffer::Stride() const
	{
		return m_desc.StructureByteStride;
	}

	D3D11Buffer::D3D11Buffer( const BUFFER_TRAIT& trait, const void* initData )
	{
		m_trait = trait;
		m_desc = ConvertTraitToDesc( m_trait );
		m_format = ConvertFormatToDxgiFormat( m_trait.m_format );
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

	D3D11Buffer::~D3D11Buffer()
	{
		delete[] m_dataStorage;
		m_dataStorage = nullptr;
	}

	void D3D11Buffer::InitResource()
	{
		CreateBuffer();
	}

	void D3D11Buffer::FreeResource()
	{
		DestroyBuffer();
	}

	void D3D11Buffer::CreateBuffer()
	{
		D3D11_SUBRESOURCE_DATA* initData = m_hasInitData ? &m_initData : nullptr;
		[[maybe_unused]] HRESULT hr = D3D11Device().CreateBuffer( &m_desc, initData, &m_buffer );
		assert( SUCCEEDED( hr ) );

		if ( HasAnyFlags( m_trait.m_miscFlag, ResourceMisc::Intermediate ) )
		{
			return;
		}

		if ( m_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE )
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_desc, m_format );
			m_srv = new D3D11ShaderResourceView( this, m_buffer, srvDesc );
			m_srv->Init();
		}

		if ( m_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS )
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_desc, m_format );
			m_uav = new D3D11UnorderedAccessView( this, m_buffer, uavDesc );
			m_uav->Init();
		}
	}

	void D3D11Buffer::DestroyBuffer()
	{
		m_srv = nullptr;
		m_uav = nullptr;

		if ( m_buffer )
		{
			[[maybe_unused]] uint32 ref = m_buffer->Release();
			m_buffer = nullptr;
		}
	}
}
