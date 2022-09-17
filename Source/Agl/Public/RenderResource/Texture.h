#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ResourceViews.h"
#include "SizedTypes.h"

#include <utility>

namespace agl
{
	class Texture : public DeviceDependantResource, public IResourceViews
	{
	public:
		AGL_DLL static RefHandle<Texture> Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );

		virtual std::pair<uint32, uint32> Size() const = 0;

		virtual ShaderResourceView* SRV() override { return m_srv.Get(); }
		virtual const ShaderResourceView* SRV() const override { return m_srv.Get(); }

		virtual UnorderedAccessView* UAV() override { return m_uav.Get(); }
		virtual const UnorderedAccessView* UAV() const override { return m_uav.Get(); }

		virtual RenderTargetView* RTV() override { return m_rtv.Get(); }
		virtual const RenderTargetView* RTV() const override { return m_rtv.Get(); }

		virtual DepthStencilView* DSV() override { return m_dsv.Get(); }
		virtual const DepthStencilView* DSV() const override { return m_dsv.Get(); }

	protected:
		RefHandle<ShaderResourceView> m_srv;
		RefHandle<UnorderedAccessView> m_uav;
		RefHandle<RenderTargetView> m_rtv;
		RefHandle<DepthStencilView> m_dsv;
	};
}
