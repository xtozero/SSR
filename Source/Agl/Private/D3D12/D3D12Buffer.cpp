#include "D3D12Buffer.h"

#include "Config/DefaultAglConfig.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"
#include "D3D12ResourceManager.h"
#include "D3D12ResourceUploader.h"
#include "D3D12ResourceViews.h"

#include "Math/Util.h"
#include "Multithread/TaskScheduler.h"

using ::agl::BufferDesc;
using ::agl::ConvertToHeapType;
using ::agl::D3D12HeapProperties;
using ::agl::ResourceAccess;
using ::agl::ResourceBindType;
using ::agl::ResourceMisc;

namespace
{
	D3D12HeapProperties ConvertToHeapProperties( const BufferDesc& desc )
	{
		D3D12HeapProperties properties = {
			.m_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.m_heapType = ConvertToHeapType( desc.m_access ),
			.m_heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
		};

		return properties;
	}

	D3D12_RESOURCE_FLAGS GetResourceFlags( const BufferDesc& desc )
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if ( HasAnyFlags( desc.m_bindType, ResourceBindType::RandomAccess ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return flags;
	}

	D3D12_RESOURCE_DESC ConvertToD3DDesc( const BufferDesc& desc )
	{
		uint64 bufferSize = desc.m_stride * desc.m_count;
		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::BufferAllowRawViews ) )
		{
			bufferSize = CalcAlignment<uint64>( bufferSize, 4 );
		}

		D3D12_RESOURCE_DESC d3dDesc = {
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
			.Flags = GetResourceFlags( desc )
		};

		return d3dDesc;
	}

	uint32 NumRawViewElements( const BufferDesc& desc )
	{
		auto bufferSize = CalcAlignment<uint32>( desc.m_stride * desc.m_count, 4 );
		return bufferSize / 4;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertToSRVDesc( const BufferDesc& desc, DXGI_FORMAT format, ID3D12Resource& resource )
	{
		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::RaytracingAccelerationStructure ) )
		{
			return D3D12_SHADER_RESOURCE_VIEW_DESC{
				.Format = DXGI_FORMAT_UNKNOWN,
				.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.RaytracingAccelerationStructure = {
					.Location = resource.GetGPUVirtualAddress()
				}
			};
		}
		else
		{
			bool bStructured = HasAnyFlags( desc.m_miscFlag, ResourceMisc::BufferStructured );
			bool bAllowRawViews = HasAnyFlags( desc.m_miscFlag, ResourceMisc::BufferAllowRawViews );

			DXGI_FORMAT actualFormat = bStructured ? DXGI_FORMAT_UNKNOWN : ( bAllowRawViews ? DXGI_FORMAT_R32_TYPELESS : format );

			return D3D12_SHADER_RESOURCE_VIEW_DESC {
				.Format = actualFormat,
				.ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Buffer = {
					.FirstElement = 0,
					.NumElements = bAllowRawViews ? NumRawViewElements( desc ) : desc.m_count,
					.StructureByteStride = bStructured ? desc.m_stride : 0,
					.Flags = bAllowRawViews ? D3D12_BUFFER_SRV_FLAG_RAW : D3D12_BUFFER_SRV_FLAG_NONE
				}
			};
		}
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC ConvertToUAVDesc( const BufferDesc& desc, DXGI_FORMAT format )
	{
		bool bStructured = HasAnyFlags( desc.m_miscFlag, ResourceMisc::BufferStructured );
		bool bAllowRawViews = HasAnyFlags( desc.m_miscFlag, ResourceMisc::BufferAllowRawViews );

		DXGI_FORMAT actualFormat = bStructured ? DXGI_FORMAT_UNKNOWN : ( bAllowRawViews ? DXGI_FORMAT_R32_TYPELESS : format );

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {
			.Format = actualFormat,
			.ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
			.Buffer = {
				.FirstElement = 0,
				.NumElements = bAllowRawViews ? NumRawViewElements( desc ) : desc.m_count,
				.StructureByteStride = bStructured ? desc.m_stride : 0,
				.CounterOffsetInBytes = 0,
				.Flags = bAllowRawViews ? D3D12_BUFFER_UAV_FLAG_RAW : D3D12_BUFFER_UAV_FLAG_NONE
			}
		};

		return uav;
	}
}

namespace agl
{
	void D3D12Buffer::CreateShaderResource()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertToSRVDesc( m_desc, m_format, *Resource() );
		m_srv = new D3D12ShaderResourceView( this, Resource(), srvDesc );
		m_srv->Init();
	}

	void D3D12Buffer::CreateUnorderedAccess()
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertToUAVDesc( m_desc, m_format );
		m_uav = new D3D12UnorderedAccessView( this, Resource(), uavDesc );
		m_uav->Init();
	}

	void* D3D12Buffer::Resource() const
	{
		return m_resourceInfo.GetResource();
	}

	ID3D12Resource* D3D12Buffer::Resource()
	{
		return m_resourceInfo.GetResource();
	}

	const AllocatedResourceInfo& D3D12Buffer::GetResourceInfo() const
	{
		return m_resourceInfo;
	}

	const D3D12_RESOURCE_DESC& D3D12Buffer::Desc() const
	{
		return m_d3dDesc;
	}

	DXGI_FORMAT D3D12Buffer::GetFormat() const
	{
		return m_format;
	}

	LockedResource D3D12Buffer::Lock( uint32 subResource, ResourceLockFlag lockFlag )
	{
		ID3D12Resource* resource = nullptr;

		if ( IsDynamic() )
		{
			if ( m_neverLocked || ( lockFlag != ResourceLockFlag::WriteDiscard ) )
			{
				resource = Resource();
				m_neverLocked = false;
			}
			else
			{
				CreateBuffer();
				resource = Resource();
			}
		}
		else
		{
			// TODO
		}

		if ( resource == nullptr )
		{
			return {};
		}

		void* mappedData = nullptr;
		[[maybe_unused]] HRESULT hr = resource->Map( subResource, nullptr, &mappedData );
		assert( SUCCEEDED( hr ) );

		LockedResource result = {
			.m_data = mappedData,
			.m_rowPitch = Size(),
			.m_depthPitch = Size()
		};

		return result;
	}

	void D3D12Buffer::UnLock( uint32 subResource )
	{
		if ( Resource() == nullptr )
		{
			return;
		}

		Resource()->Unmap( subResource, nullptr );
	}

	D3D12ConstantBufferView* D3D12Buffer::CBV() const
	{
		assert( IsInRenderThread() );

		return m_cbv.Get();
	}

	D3D12Buffer::D3D12Buffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
		: Buffer( initialState )
		, m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
		m_debugName = Name( debugName );
		m_desc = desc;
		m_format = ConvertFormatToDxgiFormat( m_desc.m_format );

		if ( initData != nullptr )
		{
			m_hasInitData = true;

			m_dataStorage = new uint8[m_d3dDesc.Width];
			std::memcpy( m_dataStorage, initData, Size() );
		}
	}

	D3D12Buffer::~D3D12Buffer()
	{
		delete[] m_dataStorage;
		m_dataStorage = nullptr;
	}

	void D3D12Buffer::SetDebugObjectName()
	{
		if ( ID3D12Resource* resource = m_resourceInfo.GetResource() )
		{
			auto dataSize = static_cast<uint32>( m_debugName.Str().size() );
			resource->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, m_debugName.Str().data() );
		}
	}

	void D3D12Buffer::CreateBuffer()
	{
		AdjustInitalResourceStates();

		D3D12HeapProperties properties = ConvertToHeapProperties( m_desc );

		D3D12ResourceAllocator& allocator = D3D12Allocator();
		m_resourceInfo = allocator.AllocateResource(
			properties,
			m_d3dDesc,
			ConvertToResourceStates( GetResourceState() )
		);

		SetDebugObjectName();

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
				D3D12Uploader().Upload( *this, m_dataStorage );
			}
		}

		if ( HasAnyFlags( m_desc.m_miscFlag, ResourceMisc::Intermediate | ResourceMisc::WithoutViews ) )
		{
			return;
		}

		if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::ShaderResource ) )
		{
			CreateShaderResource();
		}

		if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::RandomAccess ) )
		{
			CreateUnorderedAccess();
		}
	}

	void D3D12Buffer::DestroyBuffer()
	{
		m_srv = nullptr;
		m_uav = nullptr;
		m_resourceInfo = AllocatedResourceInfo();
	}

	void D3D12Buffer::InitResource()
	{
		CreateBuffer();
	}

	void D3D12Buffer::FreeResource()
	{
		DestroyBuffer();
	}

	void D3D12Buffer::AdjustInitalResourceStates()
	{
		if ( HasAllFlags( m_desc.m_access, ResourceAccess::Readback ) )
		{
			SetResourceState( ResourceState::CopyDest );
		}
		else if ( HasAllFlags( m_desc.m_access, ResourceAccess::Upload ) )
		{
			SetResourceState( ResourceState::GenericRead );
		}
		else if ( m_hasInitData && HasAllFlags( m_desc.m_access, ResourceAccess::CpuWrite ) == false )
		{
			SetResourceState( ResourceState::Common );
		}
	}

	D3D12ConstantBuffer::D3D12ConstantBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
		: D3D12Buffer( desc, debugName, initialState, initData )
	{
		m_d3dDesc.Width = CalcAlignment<uint64>( m_d3dDesc.Width, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT );
	}

	void D3D12ConstantBuffer::CreateBuffer()
	{
		D3D12Buffer::CreateBuffer();

		uint32 alignedSize = CalcAlignment<uint32>( Size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT );

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {
			.BufferLocation = Resource()->GetGPUVirtualAddress(),
			.SizeInBytes = alignedSize
		};

		m_cbv = new D3D12ConstantBufferView( this, Resource(), cbvDesc );
		m_cbv->Init();
	}

	void D3D12ConstantBuffer::DestroyBuffer()
	{
		m_cbv = nullptr;

		D3D12Buffer::DestroyBuffer();
	}

	void D3D12DisposableConstantBufferPool::Prepare()
	{
		m_top = 0;
		for ( Block& block : m_blocks )
		{
			block.m_size = 0;
		}
	}

	D3D12DisposableConstantBufferPool::AllocatedInfo D3D12DisposableConstantBufferPool::Allocate( uint32 size )
	{
		if ( ( m_top < m_blocks.size() )
			&& ( m_blocks[m_top].m_capacity - m_blocks[m_top].m_size < size ) )
		{
			++m_top;
		}

		if ( m_blocks.size() <= m_top )
		{
			CreateBlock();
		}

		Block& block = m_blocks[m_top];

		AllocatedInfo allcatedInfo = {
			.m_resourceInfo = block.m_resourceInfo,
			.m_offset = block.m_size,
			.m_lockedData = block.m_lockedData + block.m_size,
		};

		block.m_size += size;

		return allcatedInfo;
	}

	D3D12DisposableConstantBufferPool::~D3D12DisposableConstantBufferPool()
	{
		for ( Block& block : m_blocks )
		{
			if ( ID3D12Resource* resource = block.m_resourceInfo.GetResource() )
			{
				resource->Unmap( 0, nullptr );
			}

			block.m_resourceInfo = AllocatedResourceInfo();
		}
	}

	void D3D12DisposableConstantBufferPool::CreateBlock()
	{
		m_blocks.emplace_back();

		Block& block = m_blocks.back();

		D3D12HeapProperties heapProperties = {
			.m_alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.m_heapType = D3D12_HEAP_TYPE_UPLOAD,
			.m_heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS
		};

		D3D12_RESOURCE_DESC desc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = DefaultBlockSize,
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

		D3D12ResourceAllocator& allocator = D3D12Allocator();
		block.m_resourceInfo = allocator.AllocateResource(
			heapProperties,
			desc,
			D3D12_RESOURCE_STATE_GENERIC_READ
		);

		block.m_size = 0;
		block.m_capacity = DefaultBlockSize;

		ID3D12Resource* resource = block.m_resourceInfo.GetResource();
		assert( resource != nullptr );
		
		const char* debugName = "Disposable Constant Buffer";
		auto dataSize = static_cast<uint32>( std::strlen( debugName ) );
		resource->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, debugName );

		void* lockedData = nullptr;
		resource->Map( 0, nullptr, &lockedData );

		block.m_lockedData = static_cast<uint8*>( lockedData );
	}

	LockedResource D3D12DisposableConstantBuffer::Lock( [[maybe_unused]] uint32 subResource, [[maybe_unused]] ResourceLockFlag lockFlag )
	{
		return {
			.m_data = m_lockedData,
			.m_rowPitch = Size(),
			.m_depthPitch = Size()
		};
	}

	D3D12_GPU_VIRTUAL_ADDRESS D3D12DisposableConstantBuffer::GetGPUVirtualAddress() const
	{
		return m_gpuVirtualAddress;
	}

	D3D12DisposableConstantBuffer::D3D12DisposableConstantBuffer( const BufferDesc& desc, const char* debugName )
		: D3D12Buffer( desc, debugName, ResourceState::GenericRead, nullptr )
	{
	}

	void D3D12DisposableConstantBuffer::CreateBuffer()
	{
		auto& d3d12ResourceManager = *static_cast<D3D12ResourceManager*>( GetInterface<IResourceManager>() );

		D3D12DisposableConstantBufferPool& pool = d3d12ResourceManager.GetDisposableConstantBufferPool();

		uint32 alignedSize = CalcAlignment<uint32>( Size(), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT );
		auto allocatedInfo = pool.Allocate( alignedSize );

		m_resourceInfo = std::move( allocatedInfo.m_resourceInfo );
		m_gpuVirtualAddress = Resource()->GetGPUVirtualAddress() + allocatedInfo.m_offset;
		m_lockedData = allocatedInfo.m_lockedData;
	}

	void D3D12DisposableConstantBuffer::DestroyBuffer()
	{
		m_gpuVirtualAddress = {};

		D3D12Buffer::DestroyBuffer();
	}

	const D3D12_INDEX_BUFFER_VIEW& D3D12IndexBuffer::GetView() const
	{
		return m_view;
	}

	D3D12IndexBuffer::D3D12IndexBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
		: D3D12Buffer( desc, debugName, initialState, initData )
	{
	}

	void D3D12IndexBuffer::CreateBuffer()
	{
		D3D12Buffer::CreateBuffer();

		m_view = {
			.BufferLocation = Resource()->GetGPUVirtualAddress(),
			.SizeInBytes = static_cast<uint32>( m_d3dDesc.Width ),
			.Format = ( Stride() == sizeof( uint16 ) ) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT
		};
	}

	const D3D12_VERTEX_BUFFER_VIEW& D3D12VertexBuffer::GetView() const
	{
		return m_view;
	}

	D3D12VertexBuffer::D3D12VertexBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
		: D3D12Buffer( desc, debugName, initialState, initData )
	{
	}

	void D3D12VertexBuffer::CreateBuffer()
	{
		D3D12Buffer::CreateBuffer();

		m_view = {
			.BufferLocation = Resource()->GetGPUVirtualAddress(),
			.SizeInBytes = static_cast<uint32>( m_d3dDesc.Width ),
			.StrideInBytes = Stride()
		};
	}
}
