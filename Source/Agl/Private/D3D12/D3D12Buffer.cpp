#include "D3D12Buffer.h"

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

	D3D12_RESOURCE_STATES ConvertToStates( const BUFFER_TRAIT& trait )
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
		return uint32();
	}

	D3D12Buffer::D3D12Buffer( const BUFFER_TRAIT& trait, const void* initData )
	{
		m_trait = trait;
	}

	D3D12Buffer::~D3D12Buffer()
	{
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
		D3D12_RESOURCE_DESC desc = ConvertToDesc( m_trait );
		D3D12_RESOURCE_STATES states = ConvertToStates( m_trait );

		D3D12ResourceAllocator& allocator = D3D12ResourceAllocator::GetInstance();
		m_resourceInfo = allocator.AllocateResource(
			properties,
			desc,
			states
		);
	}

	void D3D12Buffer::DestroyBuffer()
	{
		EnqueueRenderTask(
			[resourceInfo = m_resourceInfo]()
			{
				D3D12ResourceAllocator& allocator = D3D12ResourceAllocator::GetInstance();
				allocator.DeallocateResource( resourceInfo );
			} );
	}
}
