#include "Physics/BodyInstance.h"

#include "Physics/BodySetup.h"
#include "Physics/CollisionUtil.h"

namespace logic
{
	void BodyInstance::InitBody( PrimitiveComponent& ownerComponent, const BodySetup& bodySetup, const Transform& bodyTransform, PhysicsScene& physicsScene )
	{
		m_physicsHandle = physicsScene.CreatePhysicBody( ownerComponent, bodyTransform );
		m_scale3D = bodyTransform.GetScale3D();

		bodySetup.AddGeometriesToPhysicsBody( *this );

		InitDynamicPropoerties();
	}

	void BodyInstance::TermBody()
	{
		GetPhysicsScene()->DeletePhysicsBody( m_physicsHandle.value() );
		m_physicsHandle.reset();
	}

	bool BodyInstance::IsValid() const
	{
		return m_physicsHandle.has_value();
	}

	PhysicsScene* BodyInstance::GetPhysicsScene() const
	{
		return m_physicsHandle ? m_physicsHandle->m_scene : nullptr;
	}

	void BodyInstance::SetMass( float mass )
	{
		assert( mass != 0.f );
		m_inverseMass = 1.f / mass;
	}

	void BodyInstance::SetLinearDamping( float linearDamping )
	{
		m_linearDamping = linearDamping;
	}

	void BodyInstance::SetAngularDamping( float angularDamping )
	{
		m_angularDamping = angularDamping;
	}

	Transform BodyInstance::GetTransform() const
	{
		if ( IsValid() )
		{
			PhysicsBody& body = GetPhysicsBody();
			return Transform( body.GetPosition(), body.GetOrientation() );
		}

		assert( false && "Invalid call - BodyInstance::GetTransform()" );
		return Transform();
	}

	void BodyInstance::SetTransform( const Transform& transform )
	{
		if ( IsValid() )
		{
			PhysicsBody& body = GetPhysicsBody();
			body.SetPosition( transform.GetTranslation() );
			body.SetOrientation( transform.GetRotation() );
		}
	}

	PhysicsHandle BodyInstance::GetPhysicsHandle() const
	{
		return m_physicsHandle.value();
	}

	const Vector& BodyInstance::GetScale3D() const
	{
		return m_scale3D;
	}

	void BodyInstance::GetScale3D( const Vector& scale )
	{
		m_scale3D = scale;
	}

	PhysicsBody& BodyInstance::GetPhysicsBody() const
	{
		assert( IsValid() );
		assert( GetPhysicsScene() );
		PhysicsScene* scene = GetPhysicsScene();
		return scene->GetPhysicsBody( m_physicsHandle.value() );
	}

	void BodyInstance::InitDynamicPropoerties()
	{
		if ( IsValid() )
		{
			PhysicsBody& body = GetPhysicsBody();

			body.SetInverseMass( m_inverseMass );

			const ICollider* shape = body.GetCollider();
			body.SetInertiaTensor( COLLISION_UTIL::CalcInertiaTensor( shape, body.GetMass() ) );

			body.SetLinearDamping( m_linearDamping );
			body.SetAngularDamping( m_angularDamping );
		}
	}
}
