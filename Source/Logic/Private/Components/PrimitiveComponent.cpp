#include "stdafx.h"
#include "Components/PrimitiveComponent.h"

#include "Scene/IScene.h"
#include "World/World.h"

void PrimitiveComponent::CreateRenderState( )
{
	SceneComponent::CreateRenderState( );
	m_pWorld->Scene( )->AddPrimitive( this );
}

void PrimitiveComponent::RemoveRenderState( )
{
	SceneComponent::RemoveRenderState( );
	m_pWorld->Scene( )->RemovePrimitive( this );
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

const CXMFLOAT4X4& PrimitiveComponent::GetRenderMatrix( )
{
	return GetTransformMatrix( );
}

bool PrimitiveComponent::ShouldCreateRenderState( ) const
{
	return true;
}
