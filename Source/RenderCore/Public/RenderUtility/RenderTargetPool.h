#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "Texture.h"

#include <vector>

namespace rendercore
{
	class PooledRenderTarget
	{
	public:
		bool IsFree() const;
		size_t GetHash() const;
		agl::RefHandle<agl::Texture> Get() const;

		explicit PooledRenderTarget( agl::Texture& texture );

	private:
		agl::TEXTURE_TRAIT m_trait;
		agl::RefHandle<agl::Texture> m_texture;
	};

	class RenderTargetPool
	{
	public:
		static RenderTargetPool& GetInstance()
		{
			static RenderTargetPool renderTargetPool;
			return renderTargetPool;
		}

		agl::RefHandle<agl::Texture> FindFreeRenderTarget( const agl::TEXTURE_TRAIT& trait );
		void Shutdown();

	private:
		std::vector<PooledRenderTarget> m_pooledRenderTargets;
	};
}
