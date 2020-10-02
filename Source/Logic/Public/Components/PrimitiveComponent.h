#pragma once

#include "Physics/Body.h"
#include "SceneComponent.h"

class PrimitiveComponent : public SceneComponent
{
public:
	using SceneComponent::SceneComponent;

	void SetMass( float mass );

	void SetLinearDamping( float linearDamping );
	void SetAngularDamping( float angularDamping );

private:
	RigidBody m_rigidBody;
};