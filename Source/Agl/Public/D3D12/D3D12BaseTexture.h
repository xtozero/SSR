#pragma once

#include "D3D12ResourceAllocator.h"
#include "Multithread/TaskScheduler.h"
#include "Texture.h"

#include <d3d12.h>
#include <vector>

namespace agl
{
	class D3D12Texture : public TextureBase
	{
	public:
		virtual void* Resource() const
		{
			return m_resourceInfo.GetResource();
		}

		const AllocatedResourceInfo& GetResourceInfo() const;

		const D3D12_RESOURCE_DESC& GetDesc() const;

		void CreateShaderResource( std::optional<ResourceFormat> overrideFormat = {} );
		void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat = {} );
		virtual void CreateRenderTarget( std::optional<ResourceFormat> overrideFormat = {} ) = 0;
		virtual void CreateDepthStencil( std::optional<ResourceFormat> overrideFormat = {} ) = 0;

		void Reconstruct( const TextureTrait& trait, const ResourceInitData* initData );

		D3D12Texture( const TextureTrait& trait, const char* debugName, const ResourceInitData* initData );
		D3D12Texture() = default;
		D3D12Texture( const D3D12Texture& ) = delete;
		D3D12Texture& operator=( const D3D12Texture& ) = delete;
		D3D12Texture( D3D12Texture&& ) = delete;
		D3D12Texture& operator=( D3D12Texture&& ) = delete;

	protected:
		AllocatedResourceInfo m_resourceInfo;

		D3D12_RESOURCE_DESC m_desc = {};
		D3D12_HEAP_PROPERTIES m_heapProperties = {};
		D3D12_HEAP_FLAGS m_heapFlags = D3D12_HEAP_FLAG_NONE;
		std::vector<D3D12_SUBRESOURCE_DATA> m_initData;

		virtual void CreateTexture() override;

	private:
		virtual void InitResource() override
		{
			if ( Resource() == nullptr )
			{
				CreateTexture();
			}

			if ( HasAnyFlags( m_trait.m_miscFlag, ResourceMisc::Intermediate | ResourceMisc::WithoutViews ) )
			{
				return;
			}

			if ( Resource() )
			{
				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::ShaderResource ) )
				{
					CreateShaderResource();
				}

				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::RandomAccess ) )
				{
					CreateUnorderedAccess();
				}

				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::RenderTarget ) )
				{
					CreateRenderTarget();
				}

				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::DepthStencil ) )
				{
					CreateDepthStencil();
				}
			}
		}

		virtual void FreeResource() override
		{
			TextureBase::FreeResource();
			m_resourceInfo.Release();
		}
	};

	class D3D12BaseTexture2D final : public D3D12Texture
	{
	public:
		virtual void CreateRenderTarget( std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateDepthStencil( std::optional<ResourceFormat> overrideFormat = {} ) override;

		D3D12BaseTexture2D( const TextureTrait& trait, const char* debugName, const ResourceInitData* initData );
		D3D12BaseTexture2D( ID3D12Resource* texture, const char* debugName, const D3D12_RESOURCE_DESC* desc = nullptr );

	private:
	};

	class D3D12BaseTexture3D final : public D3D12Texture
	{
	public:
		virtual void CreateRenderTarget( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override {};
		virtual void CreateDepthStencil( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override {};

		D3D12BaseTexture3D( const TextureTrait& trait, const char* debugName, const ResourceInitData* initData );

	private:
	};
}
