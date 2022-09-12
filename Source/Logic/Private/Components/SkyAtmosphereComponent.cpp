#include "Components/SkyAtmosphereComponent.h"

#include "Proxies/SkyAtmosphereProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

bool SkyAtmosphereComponent::ShouldCreateRenderState() const
{
	return true;
}

void SkyAtmosphereComponent::CreateRenderState()
{
	SceneComponent::CreateRenderState();

	m_skyAtmosphereProxy = new rendercore::SkyAtmospherePorxy( *this );
	m_pWorld->Scene()->AddSkyAtmosphere( m_skyAtmosphereProxy );
}

void SkyAtmosphereComponent::RemoveRenderState()
{
	SceneComponent::RemoveRenderState();

	m_pWorld->Scene()->RemoveAtomosphere( m_skyAtmosphereProxy );
	EnqueueRenderTask(
		[proxy = m_skyAtmosphereProxy]()
		{
			delete proxy;
		} );
}
