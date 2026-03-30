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
		ID3D12Resource* Resource();
		virtual void* Resource() const override;

		const AllocatedResourceInfo& GetResourceInfo() const;

		const D3D12_RESOURCE_DESC& GetD3DDesc() const;

		virtual LockedResource Lock( uint32 subResource = 0 );
		virtual void UnLock( uint32 subResource = 0 );

		virtual void CreateShaderResource( std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat = {} ) override;

		void Reconstruct( const TextureDesc& desc, const ResourceInitData* initData );

		D3D12Texture( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData );
		D3D12Texture() = default;
		D3D12Texture( const D3D12Texture& ) = delete;
		D3D12Texture& operator=( const D3D12Texture& ) = delete;
		D3D12Texture( D3D12Texture&& ) = delete;
		D3D12Texture& operator=( D3D12Texture&& ) = delete;

	protected:
		virtual void SetDebugObjectName() override;
		virtual void CreateTexture() override;

		AllocatedResourceInfo m_resourceInfo;

		D3D12_RESOURCE_DESC m_d3dDesc = {};
		D3D12_HEAP_PROPERTIES m_heapProperties = {};
		D3D12_HEAP_FLAGS m_heapFlags = D3D12_HEAP_FLAG_NONE;
		std::vector<D3D12_SUBRESOURCE_DATA> m_initData;

	private:
		virtual void InitResource() override
		{
			if ( Resource() == nullptr )
			{
				CreateTexture();
			}

			if ( HasAnyFlags( m_desc.m_miscFlag, ResourceMisc::Intermediate | ResourceMisc::WithoutViews ) )
			{
				return;
			}

			if ( Resource() )
			{
				if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::ShaderResource ) )
				{
					CreateShaderResource();
				}

				if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::RandomAccess ) )
				{
					CreateUnorderedAccess();
				}

				if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::RenderTarget ) )
				{
					CreateRenderTarget();
				}

				if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::DepthStencil ) )
				{
					CreateDepthStencil();
				}
			}
		}

		virtual void FreeResource() override
		{
			TextureBase::FreeResource();
			m_resourceInfo = AllocatedResourceInfo();
		}

		void AdjustInitalResourceStates();

		D3D12_RESOURCE_DESC GetDescForDownload() const;
	};

	class D3D12Texture2D final : public D3D12Texture
	{
	public:
		virtual void CreateRenderTarget( std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateDepthStencil( std::optional<ResourceFormat> overrideFormat = {} ) override;

		D3D12Texture2D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData );
		D3D12Texture2D( ID3D12Resource* texture, const char* debugName, const float4& clearColor, const D3D12_RESOURCE_DESC* desc = nullptr );

	private:
	};

	class D3D12Texture3D final : public D3D12Texture
	{
	public:
		virtual void CreateRenderTarget( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateDepthStencil( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override {};

		D3D12Texture3D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData );

	private:
	};
}
