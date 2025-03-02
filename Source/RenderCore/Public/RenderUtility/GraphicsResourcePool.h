#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"
#include "SparseArray.h"
#include "Texture.h"

#include <vector>

namespace rendercore
{
	class PooledResource
	{
	public:
		bool IsFree() const;

		void Tick();
		void MarkAsInUse();
		uint32 NumUnusedFrame() const;

		explicit PooledResource( agl::GraphicsApiResource& resource );
		virtual ~PooledResource() = default;
		PooledResource( const PooledResource& other ) noexcept;
		PooledResource& operator=( const PooledResource& other ) noexcept;
		PooledResource( PooledResource&& other ) noexcept;
		PooledResource& operator=( PooledResource&& other ) noexcept;

	protected:
		RefHandle<agl::GraphicsApiResource> m_resource;

	private:
		uint32 m_numUnusedFrame = 0;
	};

	class PooledBuffer final : public PooledResource
	{
	public:
		size_t GetHash() const;
		const agl::BufferTrait& GetTrait() const;
		RefHandle<agl::Buffer> Get() const;

		explicit PooledBuffer( agl::Buffer& buffer );
		PooledBuffer( const PooledBuffer& other ) noexcept;
		PooledBuffer& operator=( const PooledBuffer& other ) noexcept;
		PooledBuffer( PooledBuffer&& other ) noexcept;
		PooledBuffer& operator=( PooledBuffer&& other ) noexcept;

	private:
		agl::BufferTrait m_trait;
	};

	class PooledTexture final : public PooledResource
	{
	public:
		size_t GetHash() const;
		const agl::TextureTrait& GetTrait() const;
		RefHandle<agl::Texture> Get() const;

		explicit PooledTexture( agl::Texture& texture );
		PooledTexture( const PooledTexture& other ) noexcept;
		PooledTexture& operator=( const PooledTexture& other ) noexcept;
		PooledTexture( PooledTexture&& other ) noexcept;
		PooledTexture& operator=( PooledTexture&& other ) noexcept;

	private:
		agl::TextureTrait m_trait;
	};

	class GraphicsResourcePool final
	{
	public:
		static GraphicsResourcePool& GetInstance()
		{
			static GraphicsResourcePool renderTargetPool;
			return renderTargetPool;
		}

		void Tick();

		RefHandle<agl::Buffer> FindFreeBuffer( const agl::BufferTrait& trait, const char* debugName );
		RefHandle<agl::Texture> FindFreeTexture( const agl::TextureTrait& trait, const char* debugName );
		void Shutdown();

	private:
		template <typename T>
		void TickInternal( T& pooledResources );

		uint32 m_allocationSizeInKB = 0;
		SparseArray<PooledTexture> m_pooledTextures;
		SparseArray<PooledBuffer> m_pooledBuffers;
	};
}
