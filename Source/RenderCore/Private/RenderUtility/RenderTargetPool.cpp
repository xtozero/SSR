#include "RenderTargetPool.h"

#include "Multithread/TaskScheduler.h"

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

	agl::RefHandle<agl::Texture> PooledRenderTarget::Get() const
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

		for ( auto iter = std::begin( m_pooledRenderTargets ); iter != std::end( m_pooledRenderTargets ); ++iter )
		{
			size_t i = iter.Index();
			if ( m_pooledRenderTargets[i].NumUnusedFrame() > 2 )
			{
				m_pooledRenderTargets.RemoveAt( i );
			}
		}
	}

	agl::RefHandle<agl::Texture> RenderTargetPool::FindFreeRenderTarget( const agl::TextureTrait& trait, const char* debugName )
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

		agl::RefHandle<agl::Texture> newTexture = agl::Texture::Create( trait, debugName );
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
