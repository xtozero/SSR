#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "VertexBuffer.h"

class VertexBufferBundle
{
public:
	void Bind( VertexBuffer& vertexBuffer, uint32 slot, uint32 offset = 0 )
	{
		if ( slot >= MAX_VERTEX_SLOT )
		{
			return;
		}

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

	uint32 NumBuffer( ) const
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

	const uint32* Offsets( ) const
	{
		return m_offset;
	}

	VertexBufferBundle( ) = default;

	~VertexBufferBundle( )
	{
		for ( aga::Buffer* vertexBuffer : m_vertexBuffers )
		{
			if ( vertexBuffer )
			{
				vertexBuffer->ReleaseRef( );
			}
		}
	}

	VertexBufferBundle( const VertexBufferBundle& other ) noexcept
	{
		*this = other;
	}

	VertexBufferBundle& operator=( const VertexBufferBundle& other )
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

			for ( uint32 i = 0; i < MAX_VERTEX_SLOT; ++i )
			{
				m_vertexBuffers[i] = other.m_vertexBuffers[i];

				if ( m_vertexBuffers[i] )
				{
					m_vertexBuffers[i]->AddRef( );
				}
			}

			m_numBuffers = other.m_numBuffers;
		}

		return *this;
	}

	VertexBufferBundle( VertexBufferBundle&& other ) noexcept
	{
		*this = std::move( other );
	}

	VertexBufferBundle& operator=( VertexBufferBundle&& other ) noexcept
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

			for ( uint32 i = 0; i < MAX_VERTEX_SLOT; ++i )
			{
				m_vertexBuffers[i] = other.m_vertexBuffers[i];
				other.m_vertexBuffers[i] = nullptr;
			}

			m_numBuffers = other.m_numBuffers;
			other.m_numBuffers = 0;
		}

		return *this;
	}

	friend bool operator==( const VertexBufferBundle& lhs, const VertexBufferBundle& rhs )
	{
		return ( lhs.m_numBuffers == rhs.m_numBuffers ) && ( VertexBufferBundle::CompareBundles( lhs, rhs, lhs.m_numBuffers ) );
	}

private:
	static bool CompareBundles( const VertexBufferBundle& lhs, const VertexBufferBundle& rhs, int32 size )
	{
		while ( size-- > 0 )
		{
			if ( ( lhs.m_offset[size] != rhs.m_offset[size] ) ||
				( lhs.m_vertexBuffers[size] != rhs.m_vertexBuffers[size] ) )
			{
				return false;
			}
		}

		return true;
	}

	aga::Buffer* m_vertexBuffers[MAX_VERTEX_SLOT] = {};
	uint32 m_offset[MAX_VERTEX_SLOT] = {};
	uint32 m_numBuffers = 0;
};