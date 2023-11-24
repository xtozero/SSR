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

		agl::RefHandle<agl::Buffer> m_buffer;
		uint32 m_elementSize = 0;
		uint32 m_numElement = 0;
		bool m_isDynamic = false;
	};
}
