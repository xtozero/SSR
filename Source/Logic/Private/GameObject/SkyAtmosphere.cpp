#include "GameObject/SkyAtmosphere.h"

#include "Components/SkyAtmosphereComponent.h"
#include "GameObject/GameObjectFactory.h"

DECLARE_GAME_OBJECT( sky_atmosphere, SkyAtmosphere );

SkyAtmosphere::SkyAtmosphere()
{
	m_pSkyAtmosphereComponent = CreateComponent<SkyAtmosphereComponent>( *this, "SkyAtmosphereComponent" );
}
