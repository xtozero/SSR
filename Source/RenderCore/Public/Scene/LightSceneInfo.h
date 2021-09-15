#pragma once

#include "SizedTypes.h"

#include <cstddef>

class LightProxy;
class LightComponent;
class Scene;

class LightSceneInfo
{
public:
	LightProxy*& Proxy( )
	{
		return m_lightProxy;
	}

	uint32 ID( ) const
	{
		return m_id;
	}

	void SetID( uint32 id )
	{
		m_id = id;
	}

	LightSceneInfo( LightComponent& component, Scene& scene );

private:
	uint32 m_id = 0;
	LightProxy* m_lightProxy = nullptr;
	Scene& m_scene;
};