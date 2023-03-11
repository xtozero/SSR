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

	agl::RefHandle<agl::Texture> RenderTargetPool::FindFreeRenderTarget( const agl::TEXTURE_TRAIT& trait )
	{
		for ( PooledRenderTarget& renderTarget : m_pooledRenderTargets )
		{
			if ( renderTarget.IsFree() == false )
			{
				continue;
			}

			if ( renderTarget.GetHash() == trait.GetHash() )
			{
				return renderTarget.Get();
			}
		}

		agl::RefHandle<agl::Texture> newTexture = agl::Texture::Create( trait );
		m_pooledRenderTargets.emplace_back( *newTexture.Get() );

		EnqueueRenderTask( 
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	void RenderTargetPool::Shutdown()
	{
		m_pooledRenderTargets.clear();
	}
}
