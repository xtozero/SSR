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
		EnqueueRenderTask( 
			[renderTargets = std::move( m_pooledRenderTargets )]()
			{
			} );
	}
}
