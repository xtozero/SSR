#pragma once

#include "common.h"
#include "CommonRenderer/ISurface.h"

#include <map>
#include <memory>

class CSurfaceManager
{
public:
	ISurface* RegisterSurface( const String& surfaceName, std::unique_ptr<ISurface>&& surface );
	ISurface* FindSurface( const String& surfaceName );

	CSurfaceManager( ) = default;
	~CSurfaceManager( ) = default;

private:
	std::map<String, std::unique_ptr<ISurface>> m_surfaces;
};