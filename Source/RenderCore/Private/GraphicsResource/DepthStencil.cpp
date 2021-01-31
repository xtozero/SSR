#include "stdafx.h"
#include "GraphicsResource/DepthStencil.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

namespace rendercore
{
	void DepthStencil::UpdateBufferSize( UINT width, UINT height )
	{
		if ( ( m_depthStencil.Get( ) == nullptr )
			|| m_depthStencil->Size( ) != std::pair( width, height ) )
		{
			TEXTURE_TRAIT trait = {
				width,
				height,
				1,
				1,
				0,
				1,
				RESOURCE_FORMAT::D24_UNORM_S8_UINT,
				RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
				RESOURCE_BIND_TYPE::DEPTH_STENCIL,
				RESOURCE_MISC::NONE
			};

			m_depthStencil = aga::Texture::Create( trait );
			if ( IsInRenderThread( ) )
			{
				m_depthStencil->InitResource( );
			}
			else
			{
				EnqueueRenderTask( [depthStencil = m_depthStencil]( )
				{
					depthStencil->InitResource( );
				} );
			}
		}
	}

	void DepthStencil::Clear( float depth, UINT8 stencil )
	{
		GetInterface<IAga>( )->ClearDepthStencil( m_depthStencil.Get( ), depth, stencil );
	}
}