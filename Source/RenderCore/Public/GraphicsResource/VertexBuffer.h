#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

namespace rendercore
{
	class VertexBuffer
	{
	public:
		void* Lock();
		void Unlock();

		uint32 Size() const
		{
			return m_size;
		}

		aga::Buffer* Resource();
		const aga::Buffer* Resource() const;

		VertexBuffer( uint32 elementSize, uint32 numElement, const void* initData, bool isDynamic = false );

		VertexBuffer() = default;
		~VertexBuffer() = default;
		VertexBuffer( const VertexBuffer& ) = default;
		VertexBuffer& operator=( const VertexBuffer& ) = default;
		VertexBuffer( VertexBuffer&& ) = default;
		VertexBuffer& operator=( VertexBuffer&& ) = default;

	protected:
		void InitResource( uint32 elementSize, uint32 numElement, const void* initData );

		aga::RefHandle<aga::Buffer> m_buffer;
		uint32 m_size = 0;
		bool m_isDynamic = false;
	};
}
