#include "GameObject/StaticMeshGameObject.h"

#include "AssetLoader.h"
#include "Components/Component.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"
#include "RenderOption.h"

namespace logic
{
	DECLARE_GAME_OBJECT( static_mesh, StaticMeshGameObject );

	StaticMeshGameObject::StaticMeshGameObject()
	{
		SetRootComponent( CreateComponent<SphereComponent>( *this, "SphereComponent" ) );

		StaticMeshComponent* staticMeshComponent = CreateComponent<StaticMeshComponent>( *this, "StaticMeshComponent" );
		staticMeshComponent->AttachToComponent( m_rootComponent, AttachmentTrasformRules::KeepRelativeTransform );
	}
}
