#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ResourceViews.h"

namespace agl
{
	class Buffer : public DeviceDependantResource, public IResourceViews, public ITransitionable
	{
	public:
		AGL_DLL static RefHandle<Buffer> Create( const BUFFER_TRAIT& trait, const void* initData = nullptr );

		virtual void* Resource() const = 0;

		virtual ShaderResourceView* SRV() override { return m_srv.Get(); }
		virtual const ShaderResourceView* SRV() const override { return m_srv.Get(); }

		virtual UnorderedAccessView* UAV() override { return m_uav.Get(); }
		virtual const UnorderedAccessView* UAV() const override { return m_uav.Get(); }

		virtual RenderTargetView* RTV() override { return nullptr; }
		virtual const RenderTargetView* RTV() const override { return nullptr; }

		virtual DepthStencilView* DSV() override { return nullptr; }
		virtual const DepthStencilView* DSV() const override { return nullptr; }

		virtual ResourceState GetState() const override;
		virtual void SetState( ResourceState state ) override;

	protected:
		BUFFER_TRAIT m_trait;

		RefHandle<ShaderResourceView> m_srv;
		RefHandle<UnorderedAccessView> m_uav;

		ResourceState m_state = ResourceState::Common;
	};
}