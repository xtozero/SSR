#include "stdafx.h"
#include "Viewport.h"

#include "../RenderResource/Viewport.h"
#include "AbstractGraphicsInterface.h"
#include "Core/InterfaceFactories.h"
#include "IRenderResourceManager.h"
#include "MultiThread/EngineTaskScheduler.h"

namespace rendercore
{
	DEVICE_ERROR Viewport::Present( bool vSync )
	{
		if ( m_pViewport.Get( ) )
		{
			return m_pViewport->Present( vSync );
		}

		return DEVICE_ERROR::DEVICE_LOST;
	}

	void Viewport::Clear( const float( &color )[4] )
	{
		if ( m_pViewport.Get( ) )
		{
			m_pViewport->Clear( color );
		}
	}

	void Viewport::Bind( )
	{
		if ( m_pViewport.Get( ) )
		{
			m_pViewport->Bind( );
		}
	}

	void* Viewport::Handle( ) const
	{
		if ( m_pViewport.Get( ) )
		{
			return m_pViewport->Handle( );
		}

		return nullptr;
	}

	std::pair<uint32, uint32> Viewport::Size( ) const
	{
		if ( m_pViewport.Get( ) )
		{
			return m_pViewport->Size( );
		}

		return {};
	}

	void Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
		if ( m_pViewport.Get( ) )
		{
			m_pViewport->Resize( newSize );
		}
	}

	aga::Texture* Viewport::Texture( )
	{
		if ( m_pViewport.Get( ) )
		{
			return m_pViewport->Texture( );
		}

		return nullptr;
	}

	Viewport::Viewport( uint32 width, uint32 height, HWND hWnd, RESOURCE_FORMAT format )
	{
		m_pViewport = aga::Viewport::Create( width, height, hWnd, format );
		EnqueueRenderTask( [viewport = m_pViewport]( )
		{
			viewport->Init( );
		} );
	}

	Viewport::~Viewport( )
	{
	}
}
