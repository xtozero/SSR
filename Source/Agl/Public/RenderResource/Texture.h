#pragma once

#include "GraphicsApiResource.h"
#include "InlineMemoryAllocator.h"
#include "LibraryTool/Common.h"
#include "ResourceViews.h"
#include "SizedTypes.h"

#include <utility>

namespace agl
{
	class ICommandListBase;

	class Texture : public GraphicsApiResource, public IResourceViews, public ITransitionable
	{
	public:
		AGL_DLL static RefHandle<Texture> Create( const TextureDesc& desc, const char* debugName );
		AGL_DLL static RefHandle<Texture> Create( const TextureDesc& desc, const char* debugName, ResourceState initialState );
		AGL_DLL static RefHandle<Texture> Create( const TextureDesc& desc, const char* debugName, const ResourceInitData* initData );
		AGL_DLL static RefHandle<Texture> Create( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData );

		virtual void CreateShaderResource( std::optional<ResourceFormat> overrideFormat = {} ) = 0;
		virtual void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat = {} ) = 0;
		virtual void CreateRenderTarget( std::optional<ResourceFormat> overrideFormat = {} ) = 0;
		virtual void CreateDepthStencil( std::optional<ResourceFormat> overrideFormat = {} ) = 0;

		virtual void UpdateTextureMips( uint32 width, uint32 height, int32 mipLevels, const ResourceInitData& initData ) = 0;

		virtual std::pair<uint32, uint32> Size() const = 0;
		virtual void* Resource() const = 0;

		virtual ShaderResourceView* SRV() override { return m_srv.Get(); }
		virtual const ShaderResourceView* SRV() const override { return m_srv.Get(); }

		virtual UnorderedAccessView* UAV( uint32 mipSlice = 0 ) override;
		virtual const UnorderedAccessView* UAV( uint32 mipSlice = 0 ) const override;

		virtual RenderTargetView* RTV() override { return m_rtv.Get(); }
		virtual const RenderTargetView* RTV() const override { return m_rtv.Get(); }

		virtual DepthStencilView* DSV() override { return m_dsv.Get(); }
		virtual const DepthStencilView* DSV() const override { return m_dsv.Get(); }

		virtual ResourceState GetResourceState() const override;
		virtual void SetResourceState( ResourceState state ) override;

		AGL_DLL const TextureDesc& GetDesc() const;
		AGL_DLL bool IsCubeMap() const;

		Texture( const TextureDesc& desc, ResourceState initialState, const char* debugName ) noexcept;
		Texture() noexcept;

	protected:
		TextureDesc m_desc = {};

		RefHandle<ShaderResourceView> m_srv;
		std::vector<RefHandle<UnorderedAccessView>, InlineAllocator<RefHandle<UnorderedAccessView>, 1>> m_uav;
		RefHandle<RenderTargetView> m_rtv;
		RefHandle<DepthStencilView> m_dsv;

	private:
		ResourceState m_state = ResourceState::Common;
	};

	class TextureBase : public Texture
	{
	public:
		virtual void UpdateTextureMips( uint32 width, uint32 height, int32 mipLevels, const ResourceInitData& initData ) override;

		virtual std::pair<uint32, uint32> Size() const override
		{
			return { m_desc.m_width, m_desc.m_height };
		}

		TextureBase( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
			: Texture( desc, initialState, debugName )
		{
			if ( initData )
			{
				m_dataStorage = new uint8[initData->m_srcSize];
				std::memcpy( m_dataStorage, initData->m_srcData, initData->m_srcSize );
			}
		}
		TextureBase() = default;
		TextureBase( const TextureBase& ) = delete;
		TextureBase& operator=( const TextureBase& ) = delete;
		TextureBase( TextureBase&& ) = delete;
		TextureBase& operator=( TextureBase&& ) = delete;
		virtual ~TextureBase() override
		{
			Free();

			delete[] m_dataStorage;
			m_dataStorage = nullptr;
		}

	protected:
		virtual void FreeResource() override
		{
			m_srv = nullptr;
			m_uav.clear();
			m_rtv = nullptr;
			m_dsv = nullptr;
		}

		virtual void CreateTexture() = 0;

		uint8* m_dataStorage = nullptr;
	};

	bool IsTexture2D( const TextureDesc& desc );
	bool IsTexture3D( const TextureDesc& desc );
}
