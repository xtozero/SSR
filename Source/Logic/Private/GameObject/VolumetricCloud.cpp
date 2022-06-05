#include "GameObject/VolumetricCloud.h"

#include "Components/VolumetricCloudComponent.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

DECLARE_GAME_OBJECT( volumetric_cloud, VolumetricCloud )

VolumetricCloud::VolumetricCloud()
{
	m_volumetricCloudComponent = CreateComponent<VolumetricCloudComponent>( *this, "VolumetricCloudComponent" );
	m_rootComponent = m_volumetricCloudComponent;
}