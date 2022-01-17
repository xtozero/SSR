#include "GameObject/SkyAtmosphere.h"

#include "Components/SkyAtmosphereComponent.h"
#include "GameObject/GameObjectFactory.h"

DECLARE_GAME_OBJECT( sky_atmosphere, SkyAtmosphere );

void SkyAtmosphere::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
}

SkyAtmosphere::SkyAtmosphere()
{
	m_pSkyAtmosphereComponent = CreateComponent<SkyAtmosphereComponent>( *this );
}
