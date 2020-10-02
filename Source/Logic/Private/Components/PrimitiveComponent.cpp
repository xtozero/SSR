#include "stdafx.h"
#include "Components/PrimitiveComponent.h"

void PrimitiveComponent::SetMass( float mass )
{
	m_rigidBody.SetMass( mass );
}

void PrimitiveComponent::SetLinearDamping( float linearDamping )
{
	m_rigidBody.SetLinearDamping( linearDamping );
}

void PrimitiveComponent::SetAngularDamping( float angularDamping )
{
	m_rigidBody.SetAngularDamping( angularDamping );
}
