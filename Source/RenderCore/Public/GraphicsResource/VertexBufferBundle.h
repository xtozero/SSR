#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "VertexBuffer.h"

namespace rendercore
{
	class VertexBufferBundle final
	{
	public:
		void Bind( agl::Buffer* vertexBuffer, uint32 slot, uint32 stride = 0, uint32 offset = 0 )
		{
			if ( slot >= agl::MAX_VERTEX_SLOT )
			{
				return;
			}

			if ( m_vertexBuffers[slot] )
			{
				m_vertexBuffers[slot]->ReleaseRef();
			}

			m_vertexBuffers[slot] = vertexBuffer;
			m_strides[slot] = stride;
			m_offsets[slot] = offset;

			if ( m_vertexBuffers[slot] )
			{
				m_vertexBuffers[slot]->AddRef();
			}

			if ( slot >= m_numBuffers )
			{
				m_numBuffers = slot + 1;
			}
		}

		void Bind( VertexBuffer& vertexBuffer, uint32 slot, uint32 stride = 0, uint32 offset = 0 )
		{
			Bind( vertexBuffer.Resource(), slot, stride, offset );
		}

		uint32 NumBuffer() const
		{
			return m_numBuffers;
		}

		agl::Buffer** VertexBuffers()
		{
			return m_vertexBuffers;
		}

		agl::Buffer* const* VertexBuffers() const
		{
			return m_vertexBuffers;
		}

		const uint32* Strides() const
		{
			return m_strides;
		}

		const uint32* Offsets() const
		{
			return m_offsets;
		}

		VertexBufferBundle() = default;

		~VertexBufferBundle()
		{
			for ( agl::Buffer* vertexBuffer : m_vertexBuffers )
			{
				if ( vertexBuffer )
				{
					vertexBuffer->ReleaseRef();
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
						vb->ReleaseRef();
					}
				}

				for ( uint32 i = 0; i < agl::MAX_VERTEX_SLOT; ++i )
				{
					m_vertexBuffers[i] = other.m_vertexBuffers[i];

					if ( m_vertexBuffers[i] )
					{
						m_vertexBuffers[i]->AddRef();
					}
				}

				std::copy( std::begin( other.m_strides ), std::end( other.m_strides ), std::begin( m_strides ) );
				std::copy( std::begin( other.m_offsets ), std::end( other.m_offsets ), std::begin( m_offsets ) );

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
						vb->ReleaseRef();
					}
				}

				for ( uint32 i = 0; i < agl::MAX_VERTEX_SLOT; ++i )
				{
					m_vertexBuffers[i] = other.m_vertexBuffers[i];
					other.m_vertexBuffers[i] = nullptr;
				}

				std::copy( std::begin( other.m_strides ), std::end( other.m_strides ), std::begin( m_strides ) );
				std::copy( std::begin( other.m_offsets ), std::end( other.m_offsets ), std::begin( m_offsets ) );

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
				if ( ( lhs.m_strides[size] != rhs.m_strides[size] )
					|| ( lhs.m_offsets[size] != rhs.m_offsets[size] )
					|| ( lhs.m_vertexBuffers[size] != rhs.m_vertexBuffers[size] ) )
				{
					return false;
				}
			}

			return true;
		}

		agl::Buffer* m_vertexBuffers[agl::MAX_VERTEX_SLOT] = {};
		uint32 m_strides[agl::MAX_VERTEX_SLOT] = {};
		uint32 m_offsets[agl::MAX_VERTEX_SLOT] = {};
		uint32 m_numBuffers = 0;
	};
}
