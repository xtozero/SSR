#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "RefHandle.h"
#include "ResourceViews.h"

namespace agl
{
	class Buffer : public GraphicsApiResource, public IResourceViews, public ITransitionable
	{
	public:
		AGL_DLL static RefHandle<Buffer> Create( const BufferTrait& trait, const char* debugName );
		AGL_DLL static RefHandle<Buffer> Create( const BufferTrait& trait, const char* debugName, ResourceState initialState );
		AGL_DLL static RefHandle<Buffer> Create( const BufferTrait& trait, const char* debugName, const void* initData );
		AGL_DLL static RefHandle<Buffer> Create( const BufferTrait& trait, const char* debugName, ResourceState initialState, const void* initData );

		virtual void* Resource() const = 0;

		virtual ShaderResourceView* SRV() override { return m_srv.Get(); }
		virtual const ShaderResourceView* SRV() const override { return m_srv.Get(); }

		virtual UnorderedAccessView* UAV( [[maybe_unused]] uint32 mipSlice = 0 ) override { return m_uav.Get(); }
		virtual const UnorderedAccessView* UAV( [[maybe_unused]] uint32 mipSlice = 0 ) const override { return m_uav.Get(); }

		virtual RenderTargetView* RTV() override { return nullptr; }
		virtual const RenderTargetView* RTV() const override { return nullptr; }

		virtual DepthStencilView* DSV() override { return nullptr; }
		virtual const DepthStencilView* DSV() const override { return nullptr; }

		virtual ResourceState GetResourceState() const override;
		virtual void SetResourceState( ResourceState state ) override;

		AGL_DLL const BufferTrait& GetTrait() const;

		bool IsDynamic() const;

		explicit Buffer( ResourceState initialState ) noexcept;

	protected:
		BufferTrait m_trait = {};

		RefHandle<ShaderResourceView> m_srv;
		RefHandle<UnorderedAccessView> m_uav;

	private:
		ResourceState m_state = ResourceState::Common;
	};
}