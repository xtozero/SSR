#include "D3D12Buffer.h"

#include "Config/DefaultAglConfig.h"
#include "D3D12Api.h"
#include "D3D12ResourceViews.h"
#include "D3D12FlagConvertor.h"
#include "Math/Util.h"
#include "Multithread/TaskScheduler.h"

using ::agl::BufferTrait;
using ::agl::ConvertAccessFlagToHeapType;
using ::agl::D3D12HeapProperties;
using ::agl::ResourceBindType;
using ::agl::ResourceMisc;

namespace
{
	D3D12HeapProperties ConvertToHeapProperties( const BufferTrait& trait )
	{
		D3D12HeapProperties properties = {
			.m_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.m_heapType = ConvertAccessFlagToHeapType( trait.m_access ),
			.m_heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
		};

		return properties;
	}

	D3D12_RESOURCE_FLAGS GetResourceFlags( const BufferTrait& trait )
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if ( HasAnyFlags( trait.m_bindType, ResourceBindType::RandomAccess ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return flags;
	}

	D3D12_RESOURCE_DESC ConvertTraitToDesc( const BufferTrait& trait )
	{
		uint64 bufferSize = trait.m_stride * trait.m_count;
		
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
			.Flags = GetResourceFlags( trait )
		};

		return desc;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const BufferTrait& trait, DXGI_FORMAT format )
	{
		bool bStructured = HasAnyFlags( trait.m_miscFlag, ResourceMisc::BufferStructured );
		bool bAllowRawViews = HasAnyFlags( trait.m_miscFlag, ResourceMisc::BufferAllowRawViews );

		DXGI_FORMAT actualFormat = bStructured ? DXGI_FORMAT_UNKNOWN : ( bAllowRawViews ? DXGI_FORMAT_R32_TYPELESS : format );

		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {
			.Format = actualFormat,
			.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Buffer = {
				.FirstElement = 0,
				.NumElements = trait.m_count,
				.StructureByteStride = bStructured ? trait.m_stride : 0,
				.Flags = bAllowRawViews ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE
			}
		};

#if _DEBUG
		if ( bAllowRawViews )
		{
			assert( ( ( trait.m_stride % 4 ) == 0 ) );
		}
#endif

		return srv;
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const BufferTrait& trait, DXGI_FORMAT format )
	{
		bool bStructured = HasAnyFlags( trait.m_miscFlag, ResourceMisc::BufferStructured );
		bool bAllowRawViews = HasAnyFlags( trait.m_miscFlag, ResourceMisc::BufferAllowRawViews );

		DXGI_FORMAT actualFormat = bStructured ? DXGI_FORMAT_UNKNOWN : ( bAllowRawViews ? DXGI_FORMAT_R32_TYPELESS : format );

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {
			.Format = actualFormat,
			.ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
			.Buffer = {
				.FirstElement = 0,
				.NumElements = trait.m_count,
				.StructureByteStride = bStructured ? trait.m_stride : 0,
				.CounterOffsetInBytes = 0,
				.Flags = bAllowRawViews ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE
			}
		};

#if _DEBUG
		if ( bAllowRawViews )
		{
			assert( ( ( trait.m_stride % 4 ) == 0 ) );
		}
#endif

		return uav;
	}

	D3D12_RESOURCE_STATES ConvertToStates( [[maybe_unused]] const BufferTrait& trait )
	{
		D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_GENERIC_READ;
		return states;
	}
}

namespace agl
{
	ID3D12Resource* D3D12Buffer::Resource()
	{
		return m_resourceInfo.GetResource();
	}

	void* D3D12Buffer::Resource() const
	{
		return m_resourceInfo.GetResource();
	}

	uint32 D3D12Buffer::CurFrameOffset() const
	{
		return 0;
	}

	uint32 D3D12Buffer::Stride() const
	{
		return m_trait.m_stride;
	}

	uint32 D3D12Buffer::Size() const
	{
		return m_trait.m_stride * m_trait.m_count;
	}

	const AllocatedResourceInfo& D3D12Buffer::GetResourceInfo() const
	{
		return m_resourceInfo;
	}

	const D3D12_RESOURCE_DESC& D3D12Buffer::Desc() const
	{
		return m_desc;
	}

	D3D12Buffer::D3D12Buffer( const BufferTrait& trait, const void* initData )
		: m_desc( ConvertTraitToDesc( trait ) )
	{
		m_trait = trait;
		m_format = ConvertFormatToDxgiFormat( m_trait.m_format );

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

		D3D12ResourceAllocator& allocator = D3D12Allocator();
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

		if ( HasAnyFlags( m_trait.m_miscFlag, ResourceMisc::Intermediate ) )
		{
			return;
		}

		if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::ShaderResource ) )
		{
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_trait, m_format );
			m_srv = new D3D12ShaderResourceView( this, Resource(), srvDesc );
			m_srv->Init();
		}

		if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::RandomAccess ) )
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_trait, m_format );
			m_uav = new D3D12UnorderedAccessView( this, Resource(), uavDesc );
			m_uav->Init();
		}
	}

	void D3D12Buffer::DestroyBuffer()
	{
		m_srv = nullptr;
		m_uav = nullptr;
		m_resourceInfo.Release();
	}

	uint32 D3D12ConstantBuffer::CurFrameOffset() const
	{
		uint32 alignedSize = CalcAlignment<uint32>( Size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		return alignedSize * GetFrameIndex();
	}

	D3D12ConstantBufferView* D3D12ConstantBuffer::CBV() const
	{
		assert( IsInRenderThread() );

		return m_cbv[GetFrameIndex()].Get();
	}

	D3D12ConstantBuffer::D3D12ConstantBuffer( const BufferTrait& trait, const void* initData )
		: D3D12Buffer( trait, initData )
	{
		m_desc.Width = CalcAlignment<uint64>( m_desc.Width, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT );
		m_desc.Width *= DefaultAgl::GetBufferCount();
	}

	void D3D12ConstantBuffer::CreateBuffer()
	{
		D3D12Buffer::CreateBuffer();

		uint32 alignedSize = CalcAlignment<uint32>( Size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT );
		
		m_cbv.resize( DefaultAgl::GetBufferCount() );
		for ( uint32 i = 0; i < DefaultAgl::GetBufferCount(); ++i )
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {
				.BufferLocation = Resource()->GetGPUVirtualAddress() + ( i * alignedSize ),
				.SizeInBytes = alignedSize
			};

			m_cbv[i] = new D3D12ConstantBufferView(this, Resource(), cbvDesc);
			m_cbv[i]->Init();
		}
	}

	void D3D12ConstantBuffer::DestroyBuffer()
	{
		m_cbv.clear();

		D3D12Buffer::DestroyBuffer();
	}

	const D3D12_INDEX_BUFFER_VIEW& D3D12IndexBuffer::GetView() const
	{
		return m_view;
	}

	D3D12IndexBuffer::D3D12IndexBuffer( const BufferTrait& trait, const void* initData )
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

	const D3D12_VERTEX_BUFFER_VIEW& D3D12VertexBuffer::GetView() const
	{
		return m_view;
	}

	D3D12VertexBuffer::D3D12VertexBuffer( const BufferTrait& trait, const void* initData )
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
