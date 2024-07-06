#pragma once

#include "AbstractGraphicsInterface.h"
#include "CommandList.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "UploadBuffer.h"
#include "VertexBuffer.h"

namespace rendercore
{
	template <typename T>
	class TypedBuffer final
	{
	public:
		void Resize( uint32 newNumElement, bool bCopyPreviousData )
		{
			if ( newNumElement > m_numElement )
			{
				TypedBuffer newBuffer = TypedBuffer( newNumElement, agl::ResourceState::CopyDest );

				if ( bCopyPreviousData )
				{
					auto commandList = GetCommandList();
					commandList.CopyResource( newBuffer.m_buffer, m_buffer, Size() );
				}

				( *this ) = std::move( newBuffer );
			}
		}

		uint32 Size() const { return sizeof( T ) * m_numElement; }

		agl::Buffer* Resource()
		{
			return m_buffer.Get();
		}

		const agl::Buffer* Resource() const
		{
			return m_buffer.Get();
		}

		agl::ShaderResourceView* SRV()
		{
			return m_buffer.Get() ? m_buffer->SRV() : nullptr;
		}

		const agl::ShaderResourceView* SRV() const
		{
			return m_buffer.Get() ? m_buffer->SRV() : nullptr;
		}

		TypedBuffer( uint32 numElement, agl::ResourceState initialState, const void* initData = nullptr ) :
			m_numElement( numElement )
		{
			InitResource( initialState, initData );
		}

		TypedBuffer() = default;
		~TypedBuffer() = default;
		TypedBuffer( const TypedBuffer& ) = default;
		TypedBuffer& operator=( const TypedBuffer& ) = default;
		TypedBuffer( TypedBuffer&& ) = default;
		TypedBuffer& operator=( TypedBuffer&& ) = default;

		operator agl::Buffer* ( ) const
		{
			return m_buffer.Get();
		}

	protected:
		void InitResource( agl::ResourceState initialState, const void* initData )
		{
			if ( m_numElement > 0 )
			{
				agl::BufferTrait trait = {
					.m_stride = static_cast<uint32>( sizeof( T ) ),
					.m_count = m_numElement,
					.m_access = agl::ResourceAccessFlag::Default,
					.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
					.m_miscFlag = agl::ResourceMisc::BufferStructured,
					.m_format = agl::ResourceFormat::Unknown
				};

				m_buffer = agl::Buffer::Create( trait, "TypedBuffer", initialState, initData );
				EnqueueRenderTask(
					[buffer = m_buffer]()
					{
						buffer->Init();
					} );
			}
		}

	private:
		RefHandle<agl::Buffer> m_buffer;
		uint32 m_numElement = 0;
	};

	template <typename T>
	class TypedUploadBuffer final : public UploadBuffer
	{
	public:
		TypedUploadBuffer( uint32 numElement, const void* initData ) : UploadBuffer( sizeof( T ), numElement, initData ) {}

		TypedUploadBuffer() : UploadBuffer( sizeof( T ), 0, nullptr ) {}
		virtual ~TypedUploadBuffer() override = default;
		TypedUploadBuffer( const TypedUploadBuffer& ) = default;
		TypedUploadBuffer& operator=( const TypedUploadBuffer& ) = default;
		TypedUploadBuffer( TypedUploadBuffer&& ) = default;
		TypedUploadBuffer& operator=( TypedUploadBuffer&& ) = default;
	};
}
