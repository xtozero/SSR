#include "Physics/PhysicsScene.h"

#include "Components/PrimitiveComponent.h"
#include "Math/Transform.h"
#include "Physics/BodySetup.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollisionUtil.h"
#include "Physics/OrientedBox.h"

namespace
{
	using ::logic::AggregateGeom;
	using ::logic::BoxElem;
	using ::logic::SphereElem;

	std::vector<std::unique_ptr<ICollider>> CreateCollider( const Vector& scale3D, const AggregateGeom& geometries )
	{
		std::vector<std::unique_ptr<ICollider>> colliders;
		colliders.reserve( geometries.Size() );

		for ( const SphereElem& sphere : geometries.m_sphereElems )
		{
			SphereElem scaledSphereElem = sphere.GetScaled( scale3D );
			colliders.emplace_back( std::make_unique<BoundingSphere>( scaledSphereElem.GetCenter(), scaledSphereElem.GetRadius() ) );
		}

		for ( const BoxElem& box : geometries.m_boxElems )
		{
			BoxElem scaledBoxElem = box.GetScaled( scale3D );
			Transform transform( scaledBoxElem.GetCenter(), scaledBoxElem.GetRotation() );
			colliders.emplace_back( std::make_unique<OrientedBox>( scaledBoxElem.GetHalfSize(), transform.ToMatrix() ) );
		}

		return colliders;
	}
}

namespace logic
{
	BoxSphereBounds PhysicsBody::Bounds() const
	{
		Transform transform( GetPosition(), GetOrientation() );
		return m_bounds.TransformBy( transform.ToMatrix() );
	}

	const ICollider* PhysicsBody::GetCollider()
	{
		return m_collider.get();
	}

	void PhysicsBody::AddCollider( std::vector<std::unique_ptr<ICollider>>& colliders )
	{
		assert( colliders.size() == 1 );

		m_bounds += colliders[0]->Bounds();
		m_collider = std::move( colliders[0] );
	}

	void SceneForceGenerator::Add( const PhysicsHandle& handle, ForceGenerator* fg )
	{
		SparseArray<PhysicsHandle>& handles = m_entries[fg];
		handles.Add( handle );
	}

	void SceneForceGenerator::Remove( const PhysicsHandle& handle )
	{
		for ( auto& [forceGenerator, handles] : m_entries )
		{
			handles.Remove( handle );
		}
	}

	void SceneForceGenerator::Clear()
	{
		m_entries.clear();
	}

	void SceneForceGenerator::GenerateForces( float duration )
	{
		for ( auto& [forceGenerator, handles] : m_entries )
		{
			for ( auto& handle : handles )
			{
				PhysicsBody& body = handle.m_scene->GetPhysicsBody( handle );
				forceGenerator->UpdateForce( &body, duration );
			}
		}
	}

	void PhysicsScene::BeginFrame( float elapsedTime )
	{
		constexpr float SIMULATE_INTERVAL = 0.016f;
		m_remainingSimulateTime += elapsedTime;
		while ( m_remainingSimulateTime >= SIMULATE_INTERVAL )
		{
			PreparePhysics();
			RunPhysics( SIMULATE_INTERVAL );
			m_remainingSimulateTime -= SIMULATE_INTERVAL;
		}
	}

	void PhysicsScene::EndFrame()
	{
		for ( auto& body : m_bodies )
		{
			PrimitiveComponent* ownerComponent = body.GetOwnerComponent();
			ownerComponent->SetPosition( body.GetPosition() );
			ownerComponent->SetRotation( body.GetOrientation() );
		}
	}

	PhysicsHandle PhysicsScene::CreatePhysicBody( PrimitiveComponent& ownerComponent, const Transform& bodyTransform )
	{
		PhysicsHandle newHandle = {
			.m_id = m_bodies.AddUninitialized(),
			.m_scene = this,
		};
		PhysicsBody& newBody = m_bodies[newHandle.m_id];
		std::construct_at( &newBody );

		newBody.SetOwerComponent( &ownerComponent );
		newBody.SetPosition( bodyTransform.GetTranslation() );
		newBody.SetOrientation( bodyTransform.GetRotation() );

		m_forceGenerator.Add( newHandle, &m_gravity );

		return newHandle;
	}

	void PhysicsScene::DeletePhysicsBody( const PhysicsHandle& handle )
	{
		assert( handle.m_scene == this );
		m_forceGenerator.Remove( handle );
		m_bodies.RemoveAt( handle.m_id );
	}

	PhysicsBody& PhysicsScene::GetPhysicsBody( const PhysicsHandle& handle )
	{
		assert( handle.m_scene == this );
		return m_bodies[handle.m_id];
	}

	void PhysicsScene::PreparePhysics()
	{
		m_collisionData.Reset( m_contacts, MAX_CONTACTS );
		m_resolver.Initialize( MAX_CONTACTS * 8 );

		m_bvhTree.Clear();
		for ( auto& body : m_bodies )
		{
			m_bvhTree.Insert( &body, body.Bounds() );
		}

		for ( auto& node : m_bvhTree )
		{
			if ( node.IsLeaf() )
			{
				node.m_body->ClearAccumulators();
				node.m_body->CalculateDerivedData();
			}
		}
	}

	void PhysicsScene::RunPhysics( float elapsedTime )
	{
		m_forceGenerator.GenerateForces( elapsedTime );

		for ( auto& node : m_bvhTree )
		{
			if ( node.IsLeaf() )
			{
				if ( node.m_body->Integrate( elapsedTime ) || node.m_body->GetDirty() )
				{
					node.m_body->ResetDirty();
				}
			}
		}

		int32 usedContacts = GenerateContacts();

		m_resolver.ResolveContacts( m_contacts, usedContacts, elapsedTime );
	}

	int32 PhysicsScene::GenerateContacts()
	{
		m_collisionData.m_friction = 0.9f;
		m_collisionData.m_restitution = 0.1f;
		m_collisionData.m_tolerance = 0.1f;

		PotentialContact<PhysicsBody> candidate[MAX_CONTACTS * 8];
		uint32 nPotentialContact = m_bvhTree.GetPotentialContacts( candidate, MAX_CONTACTS * 8 );

		for ( uint32 i = 0; i < nPotentialContact; ++i )
		{
			// if collide between immovable object skip generate contacts
			if ( ( candidate[i].m_body[0]->GetInverseMass() == 0 ) && ( candidate[i].m_body[1]->GetInverseMass() == 0 ) )
			{
				continue;
			}

			COLLISION_UTIL::DetectCollisionObjectAndObject( candidate[i].m_body[0]->GetCollider(),
				candidate[i].m_body[0],
				candidate[i].m_body[1]->GetCollider(),
				candidate[i].m_body[1],
				&m_collisionData );
		}

		return m_collisionData.m_contactsCount;
	}

	void AddCollider( const PhysicsHandle& handle, const Vector& scale3D, const AggregateGeom& geometries )
	{
		std::vector<std::unique_ptr<ICollider>> colliders = CreateCollider( scale3D, geometries );

		PhysicsBody& body = handle.m_scene->GetPhysicsBody( handle );
		body.AddCollider( colliders );
	}
}
