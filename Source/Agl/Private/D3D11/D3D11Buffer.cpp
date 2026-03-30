#include "D3D11Buffer.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"
#include "D3D11ResourceViews.h"

using ::agl::BufferDesc;
using ::agl::ConvertToCpuFlag;
using ::agl::ConvertToUsage;
using ::agl::ConvertMicsToDXMisc;
using ::agl::ConvertTypeToBind;
using ::agl::ResourceMisc;

namespace
{
	D3D11_BUFFER_DESC ConvertToD3DDesc( const BufferDesc& desc )
	{
		uint32 byteWidth = desc.m_count * desc.m_stride;
		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::BufferAllowRawViews ) )
		{
			byteWidth = CalcAlignment<uint32>( byteWidth, sizeof( int32 ) );
		}

		D3D11_USAGE usage = ConvertToUsage( desc.m_access );
		uint32 bindFlag = ConvertTypeToBind( desc.m_bindType );
		uint32 cpuAccessFlag = ConvertToCpuFlag( desc.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( desc.m_miscFlag );
		uint32 structureByteStride = desc.m_stride;

		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Intermediate ) )
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
	void D3D11Buffer::CreateShaderResource()
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_d3dDesc, m_format );
		m_srv = new D3D11ShaderResourceView( this, m_buffer, srvDesc );
		m_srv->Init();
	}

	void D3D11Buffer::CreateUnorderedAccess()
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_d3dDesc, m_format );
		m_uav = new D3D11UnorderedAccessView( this, m_buffer, uavDesc );
		m_uav->Init();
	}

	void* D3D11Buffer::Resource() const
	{
		return m_buffer;
	}

	ID3D11Buffer* D3D11Buffer::Resource()
	{
		return m_buffer;
	}

	const D3D11_BUFFER_DESC& D3D11Buffer::GetD3DDesc() const
	{
		return m_d3dDesc;
	}

	D3D11Buffer::D3D11Buffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
		: Buffer( initialState )
		, m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
		m_debugName = Name( debugName );
		m_desc = desc;
		m_format = ConvertFormatToDxgiFormat( m_desc.m_format );

		if ( initData != nullptr )
		{
			m_hasInitData = true;

			m_dataStorage = new uint8[m_d3dDesc.ByteWidth];
			std::memcpy( m_dataStorage, initData, Size() );
		}
	}

	D3D11Buffer::~D3D11Buffer()
	{
		delete[] m_dataStorage;
		m_dataStorage = nullptr;
	}

	void D3D11Buffer::SetDebugObjectName()
	{
		if ( m_buffer )
		{
			auto dataSize = static_cast<uint32>( m_debugName.Str().size() );
			m_buffer->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, m_debugName.Str().data() );
		}
	}

	void D3D11Buffer::CreateBuffer()
	{
		D3D11_SUBRESOURCE_DATA initData = {
			.pSysMem = m_dataStorage,
			.SysMemPitch = m_d3dDesc.ByteWidth,
			.SysMemSlicePitch = m_d3dDesc.ByteWidth
		};

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateBuffer( &m_d3dDesc, m_hasInitData ? &initData : nullptr, &m_buffer );
		assert( SUCCEEDED( hr ) );

		SetDebugObjectName();

		if ( HasAnyFlags( m_desc.m_miscFlag, ResourceMisc::Intermediate | ResourceMisc::WithoutViews ) )
		{
			return;
		}

		if ( m_d3dDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE )
		{
			CreateShaderResource();
		}

		if ( m_d3dDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS )
		{
			CreateUnorderedAccess();
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

	void D3D11Buffer::InitResource()
	{
		CreateBuffer();
	}

	void D3D11Buffer::FreeResource()
	{
		DestroyBuffer();
	}
}
