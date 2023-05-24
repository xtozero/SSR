#include "Components/VolumetricFogComponent.h"

#include "Proxies/VolumetricFogProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

void VolumetricFogComponent::LoadProperty( const JSON::Value& json )
{
}

rendercore::VolumetricFogProxy* VolumetricFogComponent::CreateProxy()
{
	return new rendercore::VolumetricFogProxy( *this );
}

bool VolumetricFogComponent::ShouldCreateRenderState() const
{
	return true;
}

void VolumetricFogComponent::CreateRenderState()
{
	Super::CreateRenderState();

	m_pWorld->Scene()->AddVolumetricFog( this );
}

void VolumetricFogComponent::RemoveRenderState()
{
	Super::RemoveRenderState();

	m_pWorld->Scene()->RemoveVolumetricFog( this );
}
