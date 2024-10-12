#pragma once

#include "Buffer.h"
#include "Common.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace rendercore
{
	class VertexBuffer
	{
	public:
		void Resize( uint32 newNumElement, bool copyPreviousData );

		void* Lock();
		void Unlock();

		uint32 Size() const;
		uint32 ElementSize() const;

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

		VertexBuffer( uint32 elementSize, uint32 numElement, agl::ResourceState initialState, const void* initData, bool isDynamic = false );

		VertexBuffer() = default;
		virtual ~VertexBuffer() = default;
		VertexBuffer( const VertexBuffer& ) = default;
		VertexBuffer& operator=( const VertexBuffer& ) = default;
		VertexBuffer( VertexBuffer&& ) = default;
		VertexBuffer& operator=( VertexBuffer&& ) = default;

	protected:
		void InitResource( uint32 elementSize, uint32 numElement, agl::ResourceState initialState, const void* initData );

		RefHandle<agl::Buffer> m_buffer;
		uint32 m_elementSize = 0;
		uint32 m_numElement = 0;
		bool m_isDynamic = false;
	};

	class DynamicVertexBufferChunk
	{
	public:
		void InitResource();

		uint8* Allocate( uint32 sizeInBytes );
		void Commit();

		uint32 Size() const;
		uint32 AllocatedSize() const;
		uint32 FreeSize() const;

		agl::Buffer* Resource();

		uint64 GetNumFailedCommit() const;

		explicit DynamicVertexBufferChunk( uint32 sizeInBytes );

	private:
		static constexpr uint32 BufferSizeAlignment = 64 * 1024;

		RefHandle<agl::Buffer> m_buffer;
		uint32 m_sizeInBytes = 0;
		uint32 m_allocatedSizeInBytes = 0;
		void* m_lockedMemory = nullptr;
		uint64 m_numFailedCommit = 0;
	};

	class GlobalDynamicVertexBuffer
	{
	public:
		struct AllocationInfo
		{
			agl::Buffer* m_buffer = nullptr;
			void* m_lockedMemory = nullptr;
			uint32 m_offset = 0;
		};

		AllocationInfo Allocate( uint32 sizeInBytes );
		void Commit();

	private:
		std::vector<DynamicVertexBufferChunk> m_chunks;
		DynamicVertexBufferChunk* m_currentChunk = nullptr;
	};
}
