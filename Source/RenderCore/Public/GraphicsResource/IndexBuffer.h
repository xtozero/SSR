#pragma once

#include "common.h"
#include "Buffer.h"
#include "GraphicsApiResource.h"

namespace rendercore
{
	class IndexBuffer
	{
	public:
		void Resize( uint32 newNumElement, bool copyPreviousData );

		void* Lock();
		void Unlock();

		uint32 Size() const;

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

		IndexBuffer( uint32 numElement, const void* initData, bool isDWORD, bool isDynamic = false );

		IndexBuffer() = default;
		~IndexBuffer() = default;
		IndexBuffer( const IndexBuffer& ) = default;
		IndexBuffer& operator=( const IndexBuffer& ) = default;
		IndexBuffer( IndexBuffer&& ) = default;
		IndexBuffer& operator=( IndexBuffer&& ) = default;

		friend bool operator==( const IndexBuffer& lhs, const IndexBuffer& rhs )
		{
			return lhs.m_numElement == rhs.m_numElement &&
				lhs.m_isDWORD == rhs.m_isDWORD &&
				lhs.m_buffer == rhs.m_buffer;
		}

	private:
		void InitResource( const void* initData );

		uint32 m_numElement = 0;
		bool m_isDWORD = false;
		bool m_isDynamic = false;
		agl::RefHandle<agl::Buffer> m_buffer;
	};
}
