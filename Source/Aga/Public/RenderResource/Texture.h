#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ResourceViews.h"
#include "SizedTypes.h"

#include <utility>

namespace aga
{
	class Texture : public DeviceDependantResource
	{
	public:
		AGA_DLL static RefHandle<Texture> Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );

		virtual std::pair<uint32, uint32> Size( ) const = 0;

		ShaderResourceView* SRV( ) { return m_srv.Get( ); }
		const ShaderResourceView* SRV( ) const { return m_srv.Get( ); }

		UnorderedAccessView* UAV( ) { return m_uav.Get( ); }
		const UnorderedAccessView* UAV( ) const { return m_uav.Get( ); }

		RenderTargetView* RTV( ) { return m_rtv.Get( ); }
		const RenderTargetView* RTV( ) const { return m_rtv.Get( ); }

		DepthStencilView* DSV( ) { return m_dsv.Get( ); }
		const DepthStencilView* DSV( ) const { return m_dsv.Get( ); }

	protected:
		RefHandle<ShaderResourceView> m_srv;
		RefHandle<UnorderedAccessView> m_uav;
		RefHandle<RenderTargetView> m_rtv;
		RefHandle<DepthStencilView> m_dsv;
	};
}
