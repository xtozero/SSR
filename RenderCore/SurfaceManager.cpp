#include "stdafx.h"
#include "SurfaceManager.h"

ISurface* CSurfaceManager::RegisterSurface( const String& surfaceName, std::unique_ptr<ISurface>& surface )
{
	auto found = m_surfaces.find( surfaceName );

	if ( found == m_surfaces.end( ) )
	{
		auto iter = m_surfaces.emplace( surfaceName, std::move( surface ) );
		if ( iter.first != m_surfaces.end( ) )
		{
			return iter.first->second.get( );
		}
	}
	else
	{
		return found->second.get( );
	}

	return nullptr;
}

ISurface* CSurfaceManager::FindSurface( const String& surfaceName )
{
	auto found = m_surfaces.find( surfaceName );

	if ( found != m_surfaces.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}