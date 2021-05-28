#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "VertexBuffer.h"

class VertexInputStream
{
public:
	void Bind( VertexBuffer& vertexBuffer, int slot, UINT offset = 0 )
	{
		if ( m_vertexBuffers[slot] )
		{
			m_vertexBuffers[slot]->ReleaseRef( );
		}

		m_vertexBuffers[slot] = vertexBuffer.Resource( );
		m_offset[slot] = offset;

		if ( m_vertexBuffers[slot] )
		{
			m_vertexBuffers[slot]->AddRef( );
		}

		if ( slot >= m_numBuffers )
		{
			m_numBuffers = slot + 1;
		}
	}

	int NumBuffer( ) const
	{
		return m_numBuffers;
	}

	aga::Buffer** VertexBuffers( )
	{
		return m_vertexBuffers;
	}

	aga::Buffer* const* VertexBuffers( ) const
	{
		return m_vertexBuffers;
	}

	const UINT* Offsets( ) const
	{
		return m_offset;
	}

	VertexInputStream( ) = default;

	~VertexInputStream( )
	{
		for ( aga::Buffer* vertexBuffer : m_vertexBuffers )
		{
			if ( vertexBuffer )
			{
				vertexBuffer->ReleaseRef( );
			}
		}
	}

	VertexInputStream( const VertexInputStream& other ) noexcept
	{
		*this = other;
	}

	VertexInputStream& operator=( const VertexInputStream& other )
	{
		if ( this != &other )
		{
			for ( auto vb : m_vertexBuffers )
			{
				if ( vb )
				{
					vb->ReleaseRef( );
				}
			}

			for ( int i = 0; i < MAX_VERTEX_SLOT; ++i )
			{
				m_vertexBuffers[i] = other.m_vertexBuffers[i];

				if ( m_vertexBuffers[i] )
				{
					m_vertexBuffers[i]->AddRef( );
				}
			}
		}

		return *this;
	}

	VertexInputStream( VertexInputStream&& other ) noexcept
	{
		*this = std::move( other );
	}

	VertexInputStream& operator=( VertexInputStream&& other ) noexcept
	{
		if ( this != &other )
		{
			for ( auto vb : m_vertexBuffers )
			{
				if ( vb )
				{
					vb->ReleaseRef( );
				}
			}

			for ( int i = 0; i < MAX_VERTEX_SLOT; ++i )
			{
				m_vertexBuffers[i] = other.m_vertexBuffers[i];
				other.m_vertexBuffers[i] = nullptr;
			}
		}

		return *this;
	}

private:
	static constexpr int MAX_VERTEX_SLOT = 32;
	aga::Buffer* m_vertexBuffers[MAX_VERTEX_SLOT] = {};
	UINT m_offset[MAX_VERTEX_SLOT] = {};
	int m_numBuffers = 0;
};