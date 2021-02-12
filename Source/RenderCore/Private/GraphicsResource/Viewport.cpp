#include "stdafx.h"
#include "Viewport.h"

#include "../RenderResource/Viewport.h"
#include "Core/InterfaceFactories.h"
#include "IAga.h"
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

	void* Viewport::Handle( ) const
	{
		if ( m_pViewport.Get( ) )
		{
			return m_pViewport->Handle( );
		}

		return nullptr;
	}

	std::pair<UINT, UINT> Viewport::Size( ) const
	{
		if ( m_pViewport.Get( ) )
		{
			return m_pViewport->Size( );
		}

		return {};
	}

	void Viewport::Resize( const std::pair<UINT, UINT>& newSize )
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

	Viewport::Viewport( int width, int height, HWND hWnd, RESOURCE_FORMAT format )
	{
		m_pViewport = GetInterface<IAga>( )->CreateViewport( width, height, hWnd, format );
		if ( IsInRenderThread( ) )
		{
			m_pViewport->Init( );
		}
		else
		{
			EnqueueRenderTask( [viewport = m_pViewport]( )
			{
				viewport->Init( );
			} );
		}
	}

	Viewport::~Viewport( )
	{
	}
}
