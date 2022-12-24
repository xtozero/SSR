#include "D3D12Buffer.h"

#include "D3D12Api.h"
#include "D3D12ResourceViews.h"
#include "D3D12FlagConvertor.h"
#include "Math/Util.h"
#include "Multithread/TaskScheduler.h"

using ::agl::BUFFER_TRAIT;
using ::agl::ConvertAccessFlagToHeapType;
using ::agl::D3D12HeapProperties;
using ::agl::ResourceBindType;

namespace
{
	D3D12HeapProperties ConvertToHeapProperties( const BUFFER_TRAIT& trait )
	{
		D3D12HeapProperties properties = {
			.m_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.m_heapType = ConvertAccessFlagToHeapType( trait.m_access ),
			.m_heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
		};

		return properties;
	}

	D3D12_RESOURCE_DESC ConvertToDesc( const BUFFER_TRAIT& trait )
	{
		uint64 bufferSize = trait.m_stride * trait.m_count;
		if ( trait.m_bindType == ResourceBindType::ConstantBuffer )
		{
			bufferSize = CalcAlignment( bufferSize, static_cast<uint64>( D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT ) );
		}

		D3D12_RESOURCE_DESC desc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = bufferSize,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = D3D12_RESOURCE_FLAG_NONE
		};

		return desc;
	}

	D3D12_RESOURCE_STATES ConvertToStates( [[maybe_unused]] const BUFFER_TRAIT& trait )
	{
		D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_GENERIC_READ;
		return states;
	}
}

namespace agl
{
	ID3D12Resource* D3D12Buffer::Resource()
	{
		return m_resourceInfo.m_resource;
	}

	const ID3D12Resource* D3D12Buffer::Resource() const
	{
		return m_resourceInfo.m_resource;
	}

	uint32 D3D12Buffer::Stride() const
	{
		return m_trait.m_stride;
	}

	uint32 D3D12Buffer::Size() const
	{
		return m_trait.m_stride * m_trait.m_count;
	}

	D3D12Buffer::D3D12Buffer( const BUFFER_TRAIT& trait, const void* initData )
		: m_desc( ConvertToDesc( trait ) )
	{
		m_trait = trait;

		if ( initData != nullptr )
		{
			m_hasInitData = true;

			uint32 dataSize = Size();
			m_dataStorage = new unsigned char[dataSize];
			std::memcpy( m_dataStorage, initData, dataSize );
		}
	}

	D3D12Buffer::~D3D12Buffer()
	{
		delete[] m_dataStorage;
		m_dataStorage = nullptr;
	}

	void D3D12Buffer::InitResource()
	{
		CreateBuffer();
	}

	void D3D12Buffer::FreeResource()
	{
		DestroyBuffer();
	}

	void D3D12Buffer::CreateBuffer()
	{
		D3D12HeapProperties properties = ConvertToHeapProperties( m_trait );
		D3D12_RESOURCE_STATES states = ConvertToStates( m_trait );
		if ( m_hasInitData && ( properties.m_heapType != D3D12_HEAP_TYPE_UPLOAD ) )
		{
			states = D3D12_RESOURCE_STATE_COPY_DEST;
		}

		D3D12ResourceAllocator& allocator = D3D12ResourceAllocator::GetInstance();
		m_resourceInfo = allocator.AllocateResource(
			properties,
			m_desc,
			states
		);

		if ( m_hasInitData )
		{
			if ( properties.m_heapType == D3D12_HEAP_TYPE_UPLOAD )
			{
				void* data = nullptr;
				[[maybe_unused]] HRESULT hr = Resource()->Map( 0, nullptr, &data );
				assert( SUCCEEDED( hr ) );

				std::memcpy( data, m_dataStorage, Size() );

				Resource()->Unmap( 0, nullptr );
			}
			else
			{
				D3D12Uploader().Upload(*this, m_dataStorage, Size() );
			}
		}
	}

	void D3D12Buffer::DestroyBuffer()
	{
		m_srv = nullptr;
		m_uav = nullptr;

		EnqueueRenderTask(
			[resourceInfo = m_resourceInfo]()
			{
				D3D12ResourceAllocator& allocator = D3D12ResourceAllocator::GetInstance();
				allocator.DeallocateResource( resourceInfo );
			} );
	}

	D3D12ConstantBuffer::D3D12ConstantBuffer( const BUFFER_TRAIT& trait, const void* initData )
		: D3D12Buffer( trait, initData )
	{
	}

	void D3D12ConstantBuffer::CreateBuffer()
	{
		D3D12Buffer::CreateBuffer();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {
			.BufferLocation = Resource()->GetGPUVirtualAddress(),
			.SizeInBytes = static_cast<uint32>( m_desc.Width )
		};

		m_cbv = new D3D12ConstantBufferView( this, Resource(), cbvDesc );
		m_cbv->Init();
	}

	void D3D12ConstantBuffer::DestroyBuffer()
	{
		m_cbv = nullptr;

		D3D12Buffer::DestroyBuffer();
	}

	D3D12IndexBuffer::D3D12IndexBuffer( const BUFFER_TRAIT& trait, const void* initData )
		: D3D12Buffer( trait, initData )
	{
	}

	void D3D12IndexBuffer::CreateBuffer()
	{
		D3D12Buffer::CreateBuffer();

		m_view = {
			.BufferLocation = Resource()->GetGPUVirtualAddress(),
			.SizeInBytes = static_cast<uint32>( m_desc.Width ),
			.Format = ( Stride() == sizeof(uint16) ) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT
		};
	}

	D3D12VertexBuffer::D3D12VertexBuffer( const BUFFER_TRAIT& trait, const void* initData )
		: D3D12Buffer( trait, initData )
	{
	}

	void D3D12VertexBuffer::CreateBuffer()
	{
		D3D12Buffer::CreateBuffer();

		m_view = {
			.BufferLocation = Resource()->GetGPUVirtualAddress(),
			.SizeInBytes = static_cast<uint32>( m_desc.Width ),
			.StrideInBytes = Stride()
		};
	}
}
