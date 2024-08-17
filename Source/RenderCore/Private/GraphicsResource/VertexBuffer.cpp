#include "VertexBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void VertexBuffer::Resize( uint32 newNumElement, bool copyPreviousData )
	{
		if ( newNumElement > m_numElement )
		{
			VertexBuffer newBuffer( m_elementSize, newNumElement, agl::ResourceState::CopyDest, nullptr, m_isDynamic );

			if ( copyPreviousData )
			{
				auto commandList = GetCommandList();
				commandList.CopyResource( newBuffer.m_buffer, m_buffer, Size() );
			}

			( *this ) = std::move( newBuffer );
		}
	}

	void* VertexBuffer::Lock()
	{
		assert( IsInRenderThread() );
		return GraphicsInterface().Lock( m_buffer ).m_data;
	}

	void VertexBuffer::Unlock()
	{
		assert( IsInRenderThread() );
		GraphicsInterface().UnLock( m_buffer );
	}

	uint32 VertexBuffer::Size() const
	{
		return m_elementSize * m_numElement;
	}

	uint32 VertexBuffer::ElementSize() const
	{
		return m_elementSize;
	}

	agl::Buffer* VertexBuffer::Resource()
	{
		return m_buffer.Get();
	}

	const agl::Buffer* VertexBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	VertexBuffer::VertexBuffer( uint32 elementSize, uint32 numElement, agl::ResourceState initialState, const void* initData, bool isDynamic )
		: m_elementSize( elementSize )
		, m_numElement( numElement )
		, m_isDynamic( isDynamic )
	{
		InitResource( elementSize, numElement, initialState, initData );
	}

	void VertexBuffer::InitResource( uint32 elementSize, uint32 numElement, agl::ResourceState initialState, const void* initData )
	{
		agl::ResourceAccessFlag accessFlag = m_isDynamic 
			? agl::ResourceAccessFlag::Upload 
			: agl::ResourceAccessFlag::Default;

		agl::BufferTrait trait = {
			.m_stride = elementSize,
			.m_count = numElement,
			.m_access = accessFlag,
			.m_bindType = agl::ResourceBindType::VertexBuffer,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = agl::ResourceFormat::Unknown
		};

		m_buffer = agl::Buffer::Create( trait, "Vertex", initialState, initData );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}

	void DynamicVertexBufferChunk::InitResource()
	{
		agl::BufferTrait trait = {
			.m_stride = m_sizeInBytes,
			.m_count = 1,
			.m_access = agl::ResourceAccessFlag::Upload,
			.m_bindType = agl::ResourceBindType::VertexBuffer,
			.m_miscFlag = agl::ResourceMisc::None,
			.m_format = agl::ResourceFormat::Unknown
		};

		m_buffer = agl::Buffer::Create( trait, "GlobalDynamicVertex", agl::ResourceState::VertexAndConstantBuffer );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}

	uint8* DynamicVertexBufferChunk::Allocate( uint32 sizeInBytes )
	{
		assert( IsInRenderThread() );
		if ( m_lockedMemory == nullptr )
		{
			m_lockedMemory = GraphicsInterface().Lock( m_buffer ).m_data;
		}

		auto allocatedMemory = static_cast<uint8*>( m_lockedMemory ) + m_allocatedSizeInBytes;
		m_allocatedSizeInBytes += sizeInBytes;

		return allocatedMemory;
	}

	void DynamicVertexBufferChunk::Commit()
	{
		assert( IsInRenderThread() );
		if ( m_lockedMemory != nullptr )
		{
			GraphicsInterface().UnLock( m_buffer );
			m_lockedMemory = nullptr;
			m_allocatedSizeInBytes = 0;
		}
	}

	uint32 DynamicVertexBufferChunk::Size() const
	{
		return m_sizeInBytes;
	}

	uint32 DynamicVertexBufferChunk::AllocatedSize() const
	{
		return m_allocatedSizeInBytes;
	}

	uint32 DynamicVertexBufferChunk::FreeSize() const
	{
		return Size() - AllocatedSize();
	}

	agl::Buffer* DynamicVertexBufferChunk::Resource()
	{
		return m_buffer.Get();
	}

	GlobalDynamicVertexBuffer::AllocationInfo GlobalDynamicVertexBuffer::Allocate( uint32 sizeInBytes )
	{
		AllocationInfo allocationInfo;

		if ( ( m_currentChunk == nullptr ) || ( m_currentChunk->FreeSize() < sizeInBytes ) )
		{
			m_currentChunk = nullptr;

			for ( DynamicVertexBufferChunk& dynamicVertexBufferChunk : m_chunks )
			{
				if ( dynamicVertexBufferChunk.FreeSize() < sizeInBytes )
				{
					continue;
				}

				m_currentChunk = &dynamicVertexBufferChunk;
				break;
			}
		}

		if ( ( m_currentChunk == nullptr ) )
		{
			m_currentChunk = &m_chunks.emplace_back( sizeInBytes );
			m_currentChunk->InitResource();
		}

		allocationInfo.m_buffer = m_currentChunk->Resource();
		allocationInfo.m_offset = m_currentChunk->AllocatedSize();
		allocationInfo.m_lockedMemory = m_currentChunk->Allocate( sizeInBytes );

		return allocationInfo;
	}

	void GlobalDynamicVertexBuffer::Commit()
	{
		for ( DynamicVertexBufferChunk& dynamicVertexBufferChunk : m_chunks )
		{
			dynamicVertexBufferChunk.Commit();
		}

		m_currentChunk = nullptr;
	}

	DynamicVertexBufferChunk::DynamicVertexBufferChunk( uint32 sizeInBytes )
		: m_sizeInBytes( CalcAlignment( sizeInBytes, BufferSizeAlignment ) )
	{
	}
}
