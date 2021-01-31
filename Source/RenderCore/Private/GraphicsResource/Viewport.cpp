#include "stdafx.h"
#include "GraphicsResource/Viewport.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"
#include "Viewport.h"

namespace rendercore
{
	RENDERCORE_DLL DEVICE_ERROR Viewport::Present( bool vSync )
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

	std::pair<UINT, UINT> Viewport::Size( ) const
	{
		if ( m_pViewport.Get( ) )
		{
			return m_pViewport->Size( );
		}

		return {};
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
	}

	Viewport::~Viewport( )
	{
	}
}
