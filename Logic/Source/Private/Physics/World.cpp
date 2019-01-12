#include "stdafx.h"
#include "Physics/World.h"

#include "Physics/BoundingSphere.h"
#include "Physics/CollisionUtil.h"
#include "Scene/DebugOverlayManager.h"

using namespace DirectX;

void World::StartFrame( )
{
	m_collisionData.Reset( m_contacts, MAX_CONTACTS );
	m_resolver.Initialize( MAX_CONTACTS * 8 );

	for ( auto& node : m_bvhTree )
	{
		if ( node.IsLeaf( ) )
		{
			node.m_body->ClearAccumulators( );
			node.m_body->CalculateDerivedData( );
		}
	}
}

void World::RunPhysics( float duration )
{
	m_registry.UpdateForces( duration );
	
	for ( auto& node : m_bvhTree )
	{
		if ( node.IsLeaf( ) )
		{
			node.m_body->Integrate( duration );
		}
	}

	int usedContacts = GenerateContacts( );

	m_resolver.ResolveContacts( m_contacts, usedContacts, duration );
}

void World::OnObjectSpawned( ObjectRelatedRigidBody* body, const BoundingSphere& volume )
{
	m_bvhTree.Insert( body, volume );

	m_registry.Add( body, &m_gravity );
}

void World::UpdateObjectMovement( ObjectRelatedRigidBody* body, const BoundingSphere& volume )
{
	if ( auto found = m_bvhTree.Find( body ) )
	{
		delete found;
		m_bvhTree.Insert( body, volume );
	}
}

void World::DebugDrawBVH( CDebugOverlayManager& debugOverlay, unsigned int color, float duration )
{
	for ( auto iter = m_bvhTree.begin( ); iter != m_bvhTree.end( ); ++iter )
	{
		debugOverlay.AddDebugSphere( iter->m_volume.GetCenter( ), iter->m_volume.GetRadius( ), color, duration );
	}
}

int World::GenerateContacts( )
{
	m_collisionData.m_friction = 0.9f;
	m_collisionData.m_restitution = 0.1f;
	m_collisionData.m_tolerance = 0.1f;

	PotentialContact<ObjectRelatedRigidBody> candidate[MAX_CONTACTS * 8];
	int nPotentialContact = m_bvhTree.GetPotentialContacts( candidate, MAX_CONTACTS * 8 );

	for ( int i = 0; i < nPotentialContact; ++i )
	{
		assert( candidate[i].m_body[0]->m_gameObject != nullptr && candidate[i].m_body[1]->m_gameObject != nullptr );

		// if collide between immovable object skip generate contacts
		if ( ( candidate[i].m_body[0]->GetInverseMass( ) == 0 ) && ( candidate[i].m_body[1]->GetInverseMass( ) == 0 ) )
		{
			continue;
		}

		COLLISION_UTIL::DetectCollisionObjectAndObject( candidate[i].m_body[0]->m_gameObject, 
														candidate[i].m_body[0], 
														candidate[i].m_body[1]->m_gameObject, 
														candidate[i].m_body[1], 
														&m_collisionData );
	}

	return m_collisionData.m_contactsCount;
}
