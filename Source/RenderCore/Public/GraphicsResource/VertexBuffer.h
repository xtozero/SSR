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

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

		VertexBuffer( uint32 elementSize, uint32 numElement, const void* initData, bool isDynamic = false );

		VertexBuffer() = default;
		~VertexBuffer() = default;
		VertexBuffer( const VertexBuffer& ) = default;
		VertexBuffer& operator=( const VertexBuffer& ) = default;
		VertexBuffer( VertexBuffer&& ) = default;
		VertexBuffer& operator=( VertexBuffer&& ) = default;

	protected:
		void InitResource( uint32 elementSize, uint32 numElement, const void* initData );

		agl::RefHandle<agl::Buffer> m_buffer;
		uint32 m_size = 0;
		bool m_isDynamic = false;
	};
}
