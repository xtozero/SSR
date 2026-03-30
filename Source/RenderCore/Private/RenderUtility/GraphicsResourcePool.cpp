#include "GraphicsResourcePool.h"

#include "Config/DefaultRenderCoreConfig.h"
#include "Multithread/TaskScheduler.h"

namespace
{
	uint32 ComputeSizeInKB( const agl::TextureDesc& desc )
	{
		uint32 bitPerPixel = agl::BitPerPixel( desc.m_format );
		uint32 totalSize = 0;

		uint32 width = desc.m_width;
		uint32 height = desc.m_height;
		uint32 depth = desc.m_depth;

		for ( uint32 i = 0; i < desc.m_mipLevels; ++i )
		{
			totalSize += ( width * height * depth * bitPerPixel + 7 ) / 8;

			width >>= 1;
			height >>= 1;
			depth >>= 1;
		}

		return ( totalSize + 1023 ) / 1024;
	}

	uint32 ComputeSizeInKB( const agl::BufferDesc& desc )
	{
		return ( desc.m_stride * desc.m_count + 1023 ) / 1024;
	}
}

namespace rendercore
{
	bool PooledResource::IsFree() const
	{
		assert( m_resource.Get() != nullptr );
		return m_resource->GetRefCount() == 1;
	}

	void PooledResource::Tick()
	{
		++m_numUnusedFrame;
	}

	void PooledResource::MarkAsInUse()
	{
		m_numUnusedFrame = 0;
	}

	uint32 PooledResource::NumUnusedFrame() const
	{
		return m_numUnusedFrame;
	}

	PooledResource::PooledResource( agl::GraphicsApiResource& resource )
		: m_resource( &resource )
	{
	}

	PooledResource::PooledResource( const PooledResource& other ) noexcept
	{
		*this = other;
	}

	PooledResource& PooledResource::operator=( const PooledResource& other ) noexcept
	{
		if ( this != &other )
		{
			m_resource = other.m_resource;
		}

		return *this;
	}

	PooledResource::PooledResource( PooledResource&& other ) noexcept
	{
		*this = std::move( other );
	}

	PooledResource& PooledResource::operator=( PooledResource&& other ) noexcept
	{
		if ( this != &other )
		{
			m_resource = std::move( other.m_resource );
		}

		return *this;
	}

	size_t PooledBuffer::GetHash() const
	{
		return m_desc.GetHash();
	}

	const agl::BufferDesc& PooledBuffer::GetDesc() const
	{
		return m_desc;
	}

	RefHandle<agl::Buffer> PooledBuffer::Get() const
	{
		auto buffer = reinterpret_cast<agl::Buffer*>( m_resource.Get() );
		return RefHandle<agl::Buffer>( buffer );
	}

	PooledBuffer::PooledBuffer( agl::Buffer& buffer )
		: PooledResource( buffer )
		, m_desc( buffer.GetDesc() )
	{
	}

	PooledBuffer::PooledBuffer( const PooledBuffer& other ) noexcept
		: PooledResource( other )
	{
		*this = other;
	}

	PooledBuffer& PooledBuffer::operator=( const PooledBuffer& other ) noexcept
	{
		PooledResource::operator=( other );

		if ( this != &other )
		{
			m_desc = other.m_desc;
		}

		return *this;
	}

	PooledBuffer::PooledBuffer( PooledBuffer&& other ) noexcept
		: PooledResource( std::move( other ) )
	{
		*this = std::move( other );
	}

	PooledBuffer& PooledBuffer::operator=( PooledBuffer&& other ) noexcept
	{
		PooledResource::operator=( std::move( other ) );

		if ( this != &other )
		{
			m_desc = std::move( other.m_desc );
		}

		return *this;
	}

	size_t PooledTexture::GetHash() const
	{
		return m_desc.GetHash();
	}

	const agl::TextureDesc& PooledTexture::GetDesc() const
	{
		return m_desc;
	}

	RefHandle<agl::Texture> PooledTexture::Get() const
	{
		auto texture = reinterpret_cast<agl::Texture*>( m_resource.Get() );
		return RefHandle<agl::Texture>( texture );
	}

	PooledTexture::PooledTexture( agl::Texture& texture )
		: PooledResource( texture )
		, m_desc( texture.GetDesc() )
	{
	}

	PooledTexture::PooledTexture( const PooledTexture& other ) noexcept
		: PooledResource( other )
	{
		*this = other;
	}

	PooledTexture& PooledTexture::operator=( const PooledTexture& other ) noexcept
	{
		PooledResource::operator=( other );

		if ( this != &other )
		{
			m_desc = other.m_desc;
		}

		return *this;
	}

	PooledTexture::PooledTexture( PooledTexture&& other ) noexcept
		: PooledResource( std::move( other ) )
	{
		*this = std::move( other );
	}

	PooledTexture& PooledTexture::operator=( PooledTexture&& other ) noexcept
	{
		PooledResource::operator=( std::move( other ) );

		if ( this != &other )
		{
			m_desc = std::move( other.m_desc );
		}

		return *this;
	}

	void GraphicsResourcePool::Tick()
	{
		TickInternal( m_pooledTextures );
		TickInternal( m_pooledBuffers );
	}

	RefHandle<agl::Buffer> GraphicsResourcePool::FindFreeBuffer( const agl::BufferDesc& desc, const char* debugName )
	{
		for ( PooledBuffer& buffer : m_pooledBuffers )
		{
			if ( buffer.IsFree() == false )
			{
				continue;
			}

			if ( buffer.GetHash() == desc.GetHash() )
			{
				buffer.MarkAsInUse();
				buffer.Get()->Rename( Name( debugName ) );
				return buffer.Get();
			}
		}

		m_allocationSizeInKB += ComputeSizeInKB( desc );

		RefHandle<agl::Buffer> newBuffer = agl::Buffer::Create( desc, debugName );
		m_pooledBuffers.Add( *newBuffer.Get() );

		return newBuffer;
	}

	RefHandle<agl::Texture> GraphicsResourcePool::FindFreeTexture( const agl::TextureDesc& desc, const char* debugName )
	{
		for ( PooledTexture& pooledTexture : m_pooledTextures )
		{
			if ( pooledTexture.IsFree() == false )
			{
				continue;
			}

			if ( pooledTexture.GetHash() == desc.GetHash() )
			{
				pooledTexture.MarkAsInUse();
				pooledTexture.Get()->Rename( Name( debugName ) );
				return pooledTexture.Get();
			}
		}

		m_allocationSizeInKB += ComputeSizeInKB( desc );

		RefHandle<agl::Texture> newTexture = agl::Texture::Create( desc, debugName );
		m_pooledTextures.Add( *newTexture.Get() );

		return newTexture;
	}

	void GraphicsResourcePool::Shutdown()
	{
		m_pooledTextures.Clear();
		m_pooledBuffers.Clear();
	}

	template<typename T>
	void GraphicsResourcePool::TickInternal( T& pooledResources )
	{
		for ( auto& resource : pooledResources )
		{
			resource.Tick();
		}

		const uint32 renderTargetPoolMinInKB = DefaultRenderCore::GraphicsResourcePoolMin() * 1024;
		while ( m_allocationSizeInKB > renderTargetPoolMinInKB )
		{
			size_t endIndex = pooledResources.Size();
			size_t oldestIndex = endIndex;

			for ( auto iter = std::begin( pooledResources ); iter != std::end( pooledResources ); ++iter )
			{
				size_t i = iter.Index();
				if ( pooledResources[i].NumUnusedFrame() > 2 )
				{
					if ( ( oldestIndex == endIndex )
						|| ( pooledResources[i].NumUnusedFrame() > pooledResources[oldestIndex].NumUnusedFrame() ) )
					{
						oldestIndex = i;
					}
				}
			}

			if ( oldestIndex != endIndex )
			{
				m_allocationSizeInKB -= ComputeSizeInKB( pooledResources[oldestIndex].GetDesc() );
				pooledResources.RemoveAt( oldestIndex );
			}
			else
			{
				break;
			}
		}
	}
}
