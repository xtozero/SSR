#include "stdafx.h"
#include "Components/PrimitiveComponent.h"

#include "Json/Json.hpp"
#include "Physics/BodySetup.h"
#include "Scene/IScene.h"
#include "World/World.h"

namespace logic
{
	void PrimitiveComponent::SendRenderTransform()
	{
		UpdateBounds();

		m_pWorld->Scene()->UpdatePrimitiveTransform( this );

		Super::SendRenderTransform();
	}

	void PrimitiveComponent::LoadProperty( const json::Value& json )
	{
		Super::LoadProperty( json );

		if ( const json::Value* pMass = json.Find( "Mass" ) )
		{
			float mass = static_cast<float>( pMass->AsReal() );
			SetMass( mass );
		}

		if ( const json::Value* pDamping = json.Find( "Damping" ) )
		{
			const json::Value& damping = *pDamping;

			if ( damping.Size() == 2 )
			{
				SetLinearDamping( static_cast<float>( damping[0].AsReal() ) );
				SetAngularDamping( static_cast<float>( damping[1].AsReal() ) );
			}
		}
	}

	void PrimitiveComponent::SetMass( float mass )
	{
		m_bodyInstance.SetMass( mass );
	}

	void PrimitiveComponent::SetLinearDamping( float linearDamping )
	{
		m_bodyInstance.SetLinearDamping( linearDamping );
	}

	void PrimitiveComponent::SetAngularDamping( float angularDamping )
	{
		m_bodyInstance.SetAngularDamping( angularDamping );
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

	bool PrimitiveComponent::ShouldCreatePhysicsState() const
	{
		return true;
	}

	void PrimitiveComponent::OnCreatePhysicsState()
	{
		Super::OnCreatePhysicsState();

		if ( m_bodyInstance.IsValid() == false )
		{
			BodySetup* bodySetup = GetBodySetup();
			if ( bodySetup )
			{
				m_bodyInstance.InitBody( *this, *bodySetup, GetTransform(), *m_pWorld->GetPhysicsScene() );
			}
		}
	}

	void PrimitiveComponent::OnDestroyPhysicsState()
	{
		if ( m_bodyInstance.IsValid() )
		{
			m_bodyInstance.TermBody();
		}

		Super::OnDestroyPhysicsState();
	}
}
