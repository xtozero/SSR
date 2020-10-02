#include "stdafx.h"
#include "World/World.h"

#include "Physics/BoundingSphere.h"
#include "Physics/CollisionUtil.h"
#include "Scene/DebugOverlayManager.h"

using namespace DirectX;

void World::OnDeviceRestore( CGameLogic& gameLogic )
{
	for ( auto& object : m_gameObjects )
	{
		object->OnDeviceRestore( gameLogic );
	}
}

void World::PreparePhysics( )
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
	
	std::vector<ObjectRelatedRigidBody*> dirtyBody;
	for ( auto& node : m_bvhTree )
	{
		if ( node.IsLeaf( ) )
		{
			if ( node.m_body->Integrate( duration ) || node.m_body->GetDirty() )
			{
				dirtyBody.push_back( node.m_body );
				node.m_body->ResetDirty( );
			}
		}
	}

	for ( auto body : dirtyBody )
	{
		const BoundingSphere* sphere = reinterpret_cast<const BoundingSphere*>( body->GetCollider( COLLIDER::SPHERE ) );
		if ( sphere )
		{
			UpdateObjectMovement( body, *sphere );
		}
	}

	int usedContacts = GenerateContacts( );

	m_resolver.ResolveContacts( m_contacts, usedContacts, duration );
}

void World::BeginFrame( )
{
	for ( auto object = m_gameObjects.begin( ); object != m_gameObjects.end( ); )
	{
		CGameObject* candidate = object->get( );
		if ( candidate->WillRemove( ) )
		{
			// OnObjectRemoved( candidate->GetRigidBody( ) );
			object = m_gameObjects.erase( object );
		}
		else
		{
			++object;
		}
	}
}

void World::RunFrame( float duration )
{
	for ( auto& object : m_gameObjects )
	{
		object->Think( duration );
	}
}

void World::EndFrame( float duration )
{
	for ( auto& object : m_gameObjects )
	{
		object->PostThink( duration );
	}
}

void World::SpawnObject( CGameLogic& gameLogic, Owner<CGameObject*> object )
{
	object->SetID( m_gameObjects.size( ) );
	m_gameObjects.emplace_back( object );

	const BoundingSphere* sphere = reinterpret_cast<const BoundingSphere*>( object->GetCollider( COLLIDER::SPHERE ) );
	if ( sphere )
	{
		// OnObjectSpawned( object->GetRigidBody( ), *sphere );
	}
}

void World::UpdateObjectMovement( ObjectRelatedRigidBody* body, const BoundingSphere& volume )
{
	m_bvhTree.Remove( body );
	m_bvhTree.Insert( body, volume );
}

void World::DebugDrawBVH( CDebugOverlayManager& debugOverlay, unsigned int color, float duration )
{
	for ( auto node : m_bvhTree )
	{
		debugOverlay.AddDebugSphere( node.m_volume.GetCenter( ), node.m_volume.GetRadius( ), color, duration );
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
		CGameObject* gameobject[] = { candidate[i].m_body[0]->GetGameObject( ), candidate[i].m_body[1]->GetGameObject( ) };

		assert( gameobject[0] != nullptr && gameobject[1] != nullptr );

		if ( gameobject[0]->WillRemove( ) || gameobject[1]->WillRemove( ) )
		{
			continue;
		}

		// if collide between immovable object skip generate contacts
		if ( ( candidate[i].m_body[0]->GetInverseMass( ) == 0 ) && ( candidate[i].m_body[1]->GetInverseMass( ) == 0 ) )
		{
			continue;
		}

		COLLISION_UTIL::DetectCollisionObjectAndObject( candidate[i].m_body[0]->GetGameObject( ),
														candidate[i].m_body[0], 
														candidate[i].m_body[1]->GetGameObject( ),
														candidate[i].m_body[1], 
														&m_collisionData );
	}

	return m_collisionData.m_contactsCount;
}

void World::OnObjectSpawned( ObjectRelatedRigidBody* body, const BoundingSphere& volume )
{
	m_bvhTree.Insert( body, volume );
	m_registry.Add( body, &m_gravity );
}

void World::OnObjectRemoved( ObjectRelatedRigidBody* body )
{
	m_registry.Remove( body );
	m_bvhTree.Remove( body );
}