#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ResourceViews.h"
#include "SizedTypes.h"

#include <utility>

namespace agl
{
	class ICommandListBase;

	class Texture : public DeviceDependantResource, public IResourceViews
	{
	public:
		AGL_DLL static RefHandle<Texture> Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );

		virtual std::pair<uint32, uint32> Size() const = 0;
		virtual void* Resource() const = 0;

		virtual ShaderResourceView* SRV() override { return m_srv.Get(); }
		virtual const ShaderResourceView* SRV() const override { return m_srv.Get(); }

		virtual UnorderedAccessView* UAV() override { return m_uav.Get(); }
		virtual const UnorderedAccessView* UAV() const override { return m_uav.Get(); }

		virtual RenderTargetView* RTV() override { return m_rtv.Get(); }
		virtual const RenderTargetView* RTV() const override { return m_rtv.Get(); }

		virtual DepthStencilView* DSV() override { return m_dsv.Get(); }
		virtual const DepthStencilView* DSV() const override { return m_dsv.Get(); }

		void Transition( ICommandListBase& commandList, ResourceState state );

	protected:
		RefHandle<ShaderResourceView> m_srv;
		RefHandle<UnorderedAccessView> m_uav;
		RefHandle<RenderTargetView> m_rtv;
		RefHandle<DepthStencilView> m_dsv;

		ResourceState m_state = ResourceState::Common;
	};

	class TextureBase : public Texture
	{
	public:
		virtual std::pair<uint32, uint32> Size() const override
		{
			return { m_trait.m_width, m_trait.m_height };
		}

		TextureBase( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData ) : m_trait( trait )
		{
			if ( initData )
			{
				m_dataStorage = new unsigned char[initData->m_srcSize];
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
			m_uav = nullptr;
			m_rtv = nullptr;
			m_dsv = nullptr;
		}

		virtual void CreateTexture() = 0;

		TEXTURE_TRAIT m_trait = {};

		void* m_dataStorage = nullptr;
	};

	bool IsTexture1D( const TEXTURE_TRAIT& trait );
	bool IsTexture2D( const TEXTURE_TRAIT& trait );
	bool IsTexture3D( const TEXTURE_TRAIT& trait );
}
