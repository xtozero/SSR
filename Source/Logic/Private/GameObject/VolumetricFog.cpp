#include "GameObject/VolumetricFog.h"

#include "Components/SceneComponent.h"
#include "Components/VolumetricFogComponent.h"
#include "GameObject/GameObjectFactory.h"

DECLARE_GAME_OBJECT( volumetric_fog, VolumetricFog );

VolumetricFog::VolumetricFog()
{
	SetRootComponent( CreateComponent<SceneComponent>( *this, "RootComponent" ));
	CreateComponent<VolumetricFogComponent>( *this, "VolumetricFogComponent" );
}
