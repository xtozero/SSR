#pragma once

#include "common.h"
#include "Math/CXMFloat.h"

#include <map>
#include <memory>

struct SurfaceTrait
{
	CXMFLOAT4	m_ambient = { 1.f, 1.f, 1.f, 1.0f };
	CXMFLOAT4	m_diffuse = { 1.f, 1.f, 1.f, 1.0f };
	CXMFLOAT4	m_specular = { 1.f, 1.f, 1.f, 1.0f };
	float		m_roughness = 0.01f;
	float		m_specularPower = 0.f;
	float		m_pedding[2] = { 0.f, 0.f };
};

struct Surface
{
	SurfaceTrait m_trait;
	String m_diffuseTexName;
};

using SurfaceMap = std::map<String, std::unique_ptr<Surface>>;

inline Surface* RegisterSurface( SurfaceMap& surfaceMap, const String& surfaceName, std::unique_ptr<Surface>&& surface )
{
	auto found = surfaceMap.find( surfaceName );

	if ( found == surfaceMap.end( ) )
	{
		auto iter = surfaceMap.emplace( surfaceName, std::move( surface ) );
		if ( iter.first != surfaceMap.end( ) )
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

inline Surface* FindSurface( SurfaceMap& surfaceMap, const String& surfaceName )
{
	auto found = surfaceMap.find( surfaceName );

	if ( found != surfaceMap.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}