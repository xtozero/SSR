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
			return m_resourceInfo.m_resource;
		}

		D3D12Texture( const TextureTrait& trait, const ResourceInitData* initData );
		D3D12Texture() = default;
		D3D12Texture( const D3D12Texture& ) = delete;
		D3D12Texture& operator=( const D3D12Texture& ) = delete;
		D3D12Texture( D3D12Texture&& ) = delete;
		D3D12Texture& operator=( D3D12Texture&& ) = delete;

	protected:
		AllocatedResourceInfo m_resourceInfo;

		D3D12_RESOURCE_DESC m_desc;
		D3D12_HEAP_PROPERTIES m_heapProperties;
		D3D12_HEAP_FLAGS m_heapFlags;
		std::vector<D3D12_SUBRESOURCE_DATA> m_initData;

		virtual void CreateTexture() override;

		void CreateShaderResource();
		void CreateUnorderedAccess();
		virtual void CreateRenderTarget() = 0;
		virtual void CreateDepthStencil() = 0;

	private:
		virtual void InitResource() override
		{
			if ( Resource() == nullptr )
			{
				CreateTexture();
			}

			if ( HasAnyFlags( m_trait.m_miscFlag, ResourceMisc::Intermediate ) )
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

			EnqueueRenderTask(
				[resourceInfo = m_resourceInfo]()
				{
					D3D12ResourceAllocator& allocator = D3D12ResourceAllocator::GetInstance();
					allocator.DeallocateResource( resourceInfo );
				} );
		}
	};

	class D3D12BaseTexture1D final : public D3D12Texture
	{
	public:
		D3D12BaseTexture1D( const TextureTrait& trait, const ResourceInitData* initData );

	protected:
		virtual void CreateRenderTarget() override {};
		virtual void CreateDepthStencil() override {};

	private:
	};

	class D3D12BaseTexture2D final : public D3D12Texture
	{
	public:
		D3D12BaseTexture2D( const TextureTrait& trait, const ResourceInitData* initData );
		D3D12BaseTexture2D( ID3D12Resource* texture, const D3D12_RESOURCE_DESC* desc = nullptr );

	protected:
		virtual void CreateRenderTarget() override;
		virtual void CreateDepthStencil() override;

	private:
	};

	class D3D12BaseTexture3D final : public D3D12Texture
	{
	public:
		D3D12BaseTexture3D( const TextureTrait& trait, const ResourceInitData* initData );

	protected:
		virtual void CreateRenderTarget() override {};
		virtual void CreateDepthStencil() override {};

	private:
	};
}
