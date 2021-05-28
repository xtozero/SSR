#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ResourceViews.h"

namespace aga
{
	class Buffer : public DeviceDependantResource
	{
	public:
		AGA_DLL static RefHandle<Buffer> Create( const BUFFER_TRAIT& trait, const void* initData = nullptr );

		ShaderResourceView* SRV( ) { return m_srv.Get( ); }
		const ShaderResourceView* SRV( ) const { return m_srv.Get( ); }

		UnorderedAccessView* UAV( ) { return m_uav.Get( ); }
		const UnorderedAccessView* UAV( ) const { return m_uav.Get( ); }

	protected:
		RefHandle<ShaderResourceView> m_srv;
		RefHandle<UnorderedAccessView> m_uav;
	};
}