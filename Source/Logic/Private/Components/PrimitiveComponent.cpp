#include "stdafx.h"
#include "Components/PrimitiveComponent.h"

#include "Scene/IScene.h"
#include "World/World.h"

void PrimitiveComponent::SendRenderTransform()
{
	UpdateBounds();

	m_pWorld->Scene()->UpdatePrimitiveTransform( this );

	Super::SendRenderTransform();
}

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

const Matrix& PrimitiveComponent::GetRenderMatrix()
{
	return GetTransformMatrix();
}

bool PrimitiveComponent::ShouldCreateRenderState() const
{
	return true;
}

void PrimitiveComponent::CreateRenderState()
{
	Super::CreateRenderState();
	m_pWorld->Scene()->AddPrimitive( this );
}

void PrimitiveComponent::RemoveRenderState()
{
	Super::RemoveRenderState();
	m_pWorld->Scene()->RemovePrimitive( this );
}