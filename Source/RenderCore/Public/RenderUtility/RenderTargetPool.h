#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "SparseArray.h"
#include "Texture.h"

#include <vector>

namespace rendercore
{
	class PooledRenderTarget final
	{
	public:
		bool IsFree() const;
		size_t GetHash() const;
		const agl::TextureTrait& GetTrait() const;
		agl::RefHandle<agl::Texture> Get() const;

		void Tick();
		void MarkAsInUse();
		uint32 NumUnusedFrame() const;

		explicit PooledRenderTarget( agl::Texture& texture );
		PooledRenderTarget( const PooledRenderTarget& other ) noexcept;
		PooledRenderTarget& operator=( const PooledRenderTarget& other ) noexcept;
		PooledRenderTarget( PooledRenderTarget&& other ) noexcept;
		PooledRenderTarget& operator=( PooledRenderTarget&& other ) noexcept;

	private:
		uint32 m_numUnusedFrame = 0;
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

		void Tick();

		agl::RefHandle<agl::Texture> FindFreeRenderTarget( const agl::TextureTrait& trait, const char* debugName );
		void Shutdown();

	private:
		uint32 m_allocationSizeInKB = 0;
		SparseArray<PooledRenderTarget> m_pooledRenderTargets;
	};
}
