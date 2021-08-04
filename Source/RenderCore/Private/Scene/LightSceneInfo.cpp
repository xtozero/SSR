#include "stdafx.h"
#include "Scene\LightSceneInfo.h"

#include "Components\LightComponent.h"

LightSceneInfo::LightSceneInfo( LightComponent& component, Scene& scene ) : m_lightProxy( component.m_lightProxy ), m_scene( scene )
{
}
