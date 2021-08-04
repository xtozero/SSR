#pragma once
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

	std::size_t ID( ) const
	{
		return m_id;
	}

	void SetID( std::size_t id )
	{
		m_id = id;
	}

	LightSceneInfo( LightComponent& component, Scene& scene );

private:
	std::size_t m_id = 0;
	LightProxy* m_lightProxy = nullptr;
	Scene& m_scene;
};