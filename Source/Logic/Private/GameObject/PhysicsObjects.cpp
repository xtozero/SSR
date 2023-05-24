#include "GameObject/PhysicsObjects.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameObject/GameObjectFactory.h"

DECLARE_GAME_OBJECT( sphere_object, SphereObject );

SphereObject::SphereObject()
{
	auto sphereComponent = CreateComponent<SphereComponent>( *this, "SphereComponent" );
	auto staticMeshComponent = CreateComponent<StaticMeshComponent>( *this, "StaticMeshComponent" );

	staticMeshComponent->AttachToComponent( sphereComponent, AttachmentTrasformRules::KeepRelativeTransform );

	SetRootComponent( sphereComponent );
}

DECLARE_GAME_OBJECT( box_object, BoxObject );

BoxObject::BoxObject()
{
	auto boxComponent = CreateComponent<BoxComponent>( *this, "BoxComponent" );
	auto staticMeshComponent = CreateComponent<StaticMeshComponent>( *this, "StaticMeshComponent" );

	staticMeshComponent->AttachToComponent( boxComponent, AttachmentTrasformRules::KeepRelativeTransform );

	SetRootComponent( boxComponent );
}
