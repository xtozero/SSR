#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"

#include <cstddef>

namespace rendercore
{
	class UploadBuffer
	{
	public:
		void Resize( uint32 numElement, const void* initData );
		uint32 ElementSize() const { return m_elementSize; }

		template <typename T>
		T* Lock()
		{
			return static_cast<T*>( LockImple() );
		}

		void Unlock();

		aga::Buffer* Resource();
		const aga::Buffer* Resource() const;

		UploadBuffer( uint32 elementSize, uint32 numElement, const void* initData );

		UploadBuffer() = default;
		~UploadBuffer() = default;
		UploadBuffer( const UploadBuffer& ) = default;
		UploadBuffer& operator=( const UploadBuffer& ) = default;
		UploadBuffer( UploadBuffer&& ) = default;
		UploadBuffer& operator=( UploadBuffer&& ) = default;

	protected:
		void InitResource( const void* initData );

		aga::RefHandle<aga::Buffer> m_buffer;
		uint32 m_elementSize = 0;
		uint32 m_numElement = 0;

	private:
		void* LockImple();
	};
}
