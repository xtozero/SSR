#pragma once

#include "AbstractGraphicsInterface.h"
#include "ConstantBuffer.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "UploadBuffer.h"
#include "VertexBuffer.h"

namespace rendercore
{
	template <typename T>
	class TypedConstatBuffer : public ConstantBuffer
	{
	public:
		void Update( const T& data )
		{
			ConstantBuffer::Update( &data, sizeof( T ) );
		}

		TypedConstatBuffer() : ConstantBuffer( sizeof( T ) ) {}
		~TypedConstatBuffer() = default;
		TypedConstatBuffer( const TypedConstatBuffer& ) = default;
		TypedConstatBuffer& operator=( const TypedConstatBuffer& ) = default;
		TypedConstatBuffer( TypedConstatBuffer&& ) = default;
		TypedConstatBuffer& operator=( TypedConstatBuffer&& ) = default;
	};

	template <typename T>
	class TypedVertexBuffer : public VertexBuffer
	{
	public:
		TypedVertexBuffer( uint32 numElement, const void* initData ) : VertexBuffer( sizeof( T ), numElement, initData, false ) { }

		TypedVertexBuffer() = default;
		~TypedVertexBuffer() = default;
		TypedVertexBuffer( const TypedVertexBuffer& ) = default;
		TypedVertexBuffer& operator=( const TypedVertexBuffer& ) = default;
		TypedVertexBuffer( TypedVertexBuffer&& ) = default;
		TypedVertexBuffer& operator=( TypedVertexBuffer&& ) = default;
	};

	template <typename T>
	class TypedBuffer
	{
	public:
		void Resize( uint32 newNumElement, bool copyPreviousData )
		{
			if ( newNumElement > m_numElement )
			{
				TypedBuffer newBuffer = TypedBuffer( newNumElement );

				if ( copyPreviousData )
				{
					GraphicsInterface().Copy( newBuffer.m_buffer, m_buffer, Size() );
				}

				( *this ) = std::move( newBuffer );
			}
		}

		uint32 Size() const { return sizeof( T ) * m_numElement; }

		aga::Buffer* Resource()
		{
			return m_buffer.Get();
		}

		const aga::Buffer* Resource() const
		{
			return m_buffer.Get();
		}

		aga::ShaderResourceView* SRV()
		{
			return m_buffer.Get() ? m_buffer->SRV() : nullptr;
		}

		const aga::ShaderResourceView* SRV() const
		{
			return m_buffer.Get() ? m_buffer->SRV() : nullptr;
		}

		TypedBuffer( uint32 numElement, const void* initData = nullptr ) :
			m_numElement( numElement )
		{
			InitResource( initData );
		}

		TypedBuffer() = default;
		~TypedBuffer() = default;
		TypedBuffer( const TypedBuffer& ) = default;
		TypedBuffer& operator=( const TypedBuffer& ) = default;
		TypedBuffer( TypedBuffer&& ) = default;
		TypedBuffer& operator=( TypedBuffer&& ) = default;

		operator aga::Buffer* ( ) const
		{
			return m_buffer.Get();
		}

	protected:
		void InitResource( const void* initData )
		{
			if ( m_numElement > 0 )
			{
				BUFFER_TRAIT trait = {
				static_cast<uint32>( sizeof( T ) ),
				m_numElement,
				RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
				RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::RANDOM_ACCESS,
				RESOURCE_MISC::BUFFER_STRUCTURED,
				RESOURCE_FORMAT::UNKNOWN
				};

				m_buffer = aga::Buffer::Create( trait, initData );
				EnqueueRenderTask(
					[buffer = m_buffer]()
					{
						buffer->Init();
					} );
			}
		}

	private:
		aga::RefHandle<aga::Buffer> m_buffer;
		uint32 m_numElement = 0;
	};

	template <typename T>
	class TypedUploadBuffer : public UploadBuffer
	{
	public:
		TypedUploadBuffer( uint32 numElement, const void* initData ) : UploadBuffer( sizeof( T ), numElement, initData ) {}

		TypedUploadBuffer() : UploadBuffer( sizeof( T ), 0, nullptr ) {}
		~TypedUploadBuffer() = default;
		TypedUploadBuffer( const TypedUploadBuffer& ) = default;
		TypedUploadBuffer& operator=( const TypedUploadBuffer& ) = default;
		TypedUploadBuffer( TypedUploadBuffer&& ) = default;
		TypedUploadBuffer& operator=( TypedUploadBuffer&& ) = default;
	};
}
