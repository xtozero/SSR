#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "Texture.h"

#include <vector>

namespace rendercore
{
	class PooledRenderTarget final
	{
	public:
		bool IsFree() const;
		size_t GetHash() const;
		agl::RefHandle<agl::Texture> Get() const;

		explicit PooledRenderTarget( agl::Texture& texture );
		PooledRenderTarget( const PooledRenderTarget& other ) noexcept;
		PooledRenderTarget& operator=( const PooledRenderTarget& other ) noexcept;
		PooledRenderTarget( PooledRenderTarget&& other ) noexcept;
		PooledRenderTarget& operator=( PooledRenderTarget&& other ) noexcept;

	private:
		agl::TextureTrait m_trait;
		agl::RefHandle<agl::Texture> m_texture;
	};

	class RenderTargetPool final
	{
	public:
		static RenderTargetPool& GetInstance()
		{
			static RenderTargetPool renderTargetPool;
			return renderTargetPool;
		}

		agl::RefHandle<agl::Texture> FindFreeRenderTarget( const agl::TextureTrait& trait, const char* debugName );
		void Shutdown();

	private:
		std::vector<PooledRenderTarget> m_pooledRenderTargets;
	};
}
