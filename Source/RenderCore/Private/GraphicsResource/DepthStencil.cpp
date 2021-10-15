#include "stdafx.h"
#include "GraphicsResource/DepthStencil.h"

#include "AbstractGraphicsInterface.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void DepthStencil::UpdateBufferSize( uint32 width, uint32 height )
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
			EnqueueRenderTask( [depthStencil = m_depthStencil]( )
			{
				depthStencil->Init( );
			} );
		}
	}

	void DepthStencil::Clear( float depth, uint8 stencil )
	{
		GraphicsInterface( ).ClearDepthStencil( m_depthStencil.Get( ), depth, stencil );
	}
}