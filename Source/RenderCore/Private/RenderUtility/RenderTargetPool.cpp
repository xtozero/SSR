#include "RenderTargetPool.h"

#include "Config/DefaultRenderCoreConfig.h"
#include "Multithread/TaskScheduler.h"

namespace
{
	uint32 ComputeSizeInKB( const agl::TextureTrait& trait )
	{
		uint32 bitPerPixel = agl::BitPerPixel( trait.m_format );
		uint32 totalSize = 0;

		uint32 width = trait.m_width;
		uint32 height = trait.m_height;
		uint32 depth = trait.m_depth;

		for ( uint32 i = 0; i < trait.m_mipLevels; ++i )
		{
			totalSize += ( width * height * depth * bitPerPixel + 7 ) / 8;

			width >>= 1;
			height >>= 1;
			depth >>= 1;
		}

		return ( totalSize + 1023 ) / 1024;
	}
}

namespace rendercore
{
	bool PooledRenderTarget::IsFree() const
	{
		assert( m_texture.Get() != nullptr );
		return m_texture->GetRefCount() == 1;
	}

	size_t PooledRenderTarget::GetHash() const
	{
		return m_trait.GetHash();
	}

	const agl::TextureTrait& PooledRenderTarget::GetTrait() const
	{
		return m_trait;
	}

	RefHandle<agl::Texture> PooledRenderTarget::Get() const
	{
		return m_texture;
	}

	void PooledRenderTarget::Tick()
	{
		++m_numUnusedFrame;
	}

	void PooledRenderTarget::MarkAsInUse()
	{
		m_numUnusedFrame = 0;
	}

	uint32 PooledRenderTarget::NumUnusedFrame() const
	{
		return m_numUnusedFrame;
	}

	PooledRenderTarget::PooledRenderTarget( agl::Texture& texture )
		: m_texture( &texture )
		, m_trait( texture.GetTrait() )
	{
	}
	PooledRenderTarget::PooledRenderTarget( const PooledRenderTarget& other ) noexcept
	{
		*this = other;
	}

	PooledRenderTarget& PooledRenderTarget::operator=( const PooledRenderTarget& other ) noexcept
	{
		if ( this != &other )
		{
			m_trait = other.m_trait;
			m_texture = other.m_texture;
		}

		return *this;
	}

	PooledRenderTarget::PooledRenderTarget( PooledRenderTarget&& other ) noexcept
	{
		*this = std::move( other );
	}

	PooledRenderTarget& PooledRenderTarget::operator=( PooledRenderTarget&& other ) noexcept
	{
		if ( this != &other )
		{
			m_trait = std::move( other.m_trait );
			m_texture = std::move( other.m_texture );
		}

		return *this;
	}

	void RenderTargetPool::Tick()
	{
		for ( PooledRenderTarget& renderTarget : m_pooledRenderTargets )
		{
			renderTarget.Tick();
		}

		uint32 renderTargetPoolMinInKB = DefaultRenderCore::RenderTargetPoolMin() * 1024;
		while ( m_allocationSizeInKB > renderTargetPoolMinInKB )
		{
			size_t endIndex = m_pooledRenderTargets.Size();
			size_t oldestIndex = endIndex;

			for ( auto iter = std::begin( m_pooledRenderTargets ); iter != std::end( m_pooledRenderTargets ); ++iter )
			{
				size_t i = iter.Index();
				if ( m_pooledRenderTargets[i].NumUnusedFrame() > 2 )
				{
					if ( ( oldestIndex == endIndex ) 
						|| ( m_pooledRenderTargets[i].NumUnusedFrame() > m_pooledRenderTargets[oldestIndex].NumUnusedFrame() ) )
					{
						oldestIndex = i;
					}
				}
			}

			if ( oldestIndex != endIndex )
			{
				m_allocationSizeInKB -= ComputeSizeInKB( m_pooledRenderTargets[oldestIndex].GetTrait() );
				m_pooledRenderTargets.RemoveAt( oldestIndex );
			}
			else
			{
				break;
			}
		}
	}

	RefHandle<agl::Texture> RenderTargetPool::FindFreeRenderTarget( const agl::TextureTrait& trait, const char* debugName )
	{
		assert( HasAnyFlags( trait.m_bindType, agl::ResourceBindType::RenderTarget | agl::ResourceBindType::DepthStencil ) );
		for ( PooledRenderTarget& renderTarget : m_pooledRenderTargets )
		{
			if ( renderTarget.IsFree() == false )
			{
				continue;
			}

			if ( renderTarget.GetHash() == trait.GetHash() )
			{
				renderTarget.MarkAsInUse();
				return renderTarget.Get();
			}
		}

		m_allocationSizeInKB += ComputeSizeInKB( trait );

		RefHandle<agl::Texture> newTexture = agl::Texture::Create( trait, debugName );
		m_pooledRenderTargets.Add( *newTexture.Get() );

		EnqueueRenderTask( 
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	void RenderTargetPool::Shutdown()
	{
		m_pooledRenderTargets.Clear();
	}
}
