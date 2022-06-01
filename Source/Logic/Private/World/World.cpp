#include "stdafx.h"
#include "World/World.h"

#include "GameObject/Player.h"
#include "InterfaceFactories.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollisionUtil.h"
#include "Renderer/IRenderCore.h"
#include "Scene/DebugOverlayManager.h"
#include "TaskScheduler.h"

using namespace DirectX;

void World::OnDeviceRestore( CGameLogic& gameLogic )
{
	for ( auto& object : m_gameObjects )
	{
		object->OnDeviceRestore( gameLogic );
	}
}

void World::Initialize()
{
	m_scene = GetInterface<IRenderCore>()->CreateScene();
}

void World::CleanUp()
{
	m_gameObjects.clear();
	GetInterface<IRenderCore>()->RemoveScene( m_scene );

	WaitRenderThread();
}

void World::Pause()
{
	m_clock.Pause();
}

void World::Resume()
{
	m_clock.Resume();
}

void World::PreparePhysics()
{
	m_collisionData.Reset( m_contacts, MAX_CONTACTS );
	m_resolver.Initialize( MAX_CONTACTS * 8 );

	for ( auto& node : m_bvhTree )
	{
		if ( node.IsLeaf() )
		{
			node.m_body->ClearAccumulators();
			node.m_body->CalculateDerivedData();
		}
	}
}

void World::RunPhysics( float duration )
{
	m_registry.UpdateForces( duration );

	std::vector<ObjectRelatedRigidBody*> dirtyBody;
	for ( auto& node : m_bvhTree )
	{
		if ( node.IsLeaf() )
		{
			if ( node.m_body->Integrate( duration ) || node.m_body->GetDirty() )
			{
				dirtyBody.push_back( node.m_body );
				node.m_body->ResetDirty();
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

	int32 usedContacts = GenerateContacts();

	m_resolver.ResolveContacts( m_contacts, usedContacts, duration );
}

void World::BeginFrame()
{
	m_clock.Tick();

	for ( auto object = m_gameObjects.begin(); object != m_gameObjects.end(); )
	{
		CGameObject* candidate = object->get();
		if ( candidate->WillRemove() )
		{
			// OnObjectRemoved( candidate->GetRigidBody( ) );
			object = m_gameObjects.erase( object );
		}
		else
		{
			++object;
		}
	}

	float totalTime = GetTimer().GetTotalTime();
	m_thinkTaskManager.BeginFrame( totalTime );
}

void World::RunFrame()
{
	RunThinkGroup( ThinkingGroup::PrePhysics );
	RunThinkGroup( ThinkingGroup::StartPhysics );
	RunThinkGroup( ThinkingGroup::DuringPhysics );
	RunThinkGroup( ThinkingGroup::EndPhysics );
}

void World::EndFrame()
{
	RunThinkGroup( ThinkingGroup::PostPhysics );
}

void World::SpawnObject( CGameLogic& gameLogic, Owner<CGameObject*> object )
{
	object->Initialize( gameLogic, *this );
	object->SetID( m_gameObjects.size() );
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

void World::DebugDrawBVH( CDebugOverlayManager& debugOverlay, uint32 color, float duration )
{
	for ( auto node : m_bvhTree )
	{
		debugOverlay.AddDebugSphere( node.m_volume.GetCenter(), node.m_volume.GetRadius(), color, duration );
	}
}

ThinkTaskManager& World::GetThinkTaskManager()
{
	return m_thinkTaskManager;
}

int32 World::GenerateContacts()
{
	m_collisionData.m_friction = 0.9f;
	m_collisionData.m_restitution = 0.1f;
	m_collisionData.m_tolerance = 0.1f;

	PotentialContact<ObjectRelatedRigidBody> candidate[MAX_CONTACTS * 8];
	uint32 nPotentialContact = m_bvhTree.GetPotentialContacts( candidate, MAX_CONTACTS * 8 );

	for ( uint32 i = 0; i < nPotentialContact; ++i )
	{
		CGameObject* gameobject[] = { candidate[i].m_body[0]->GetGameObject(), candidate[i].m_body[1]->GetGameObject() };

		assert( gameobject[0] != nullptr && gameobject[1] != nullptr );

		if ( gameobject[0]->WillRemove() || gameobject[1]->WillRemove() )
		{
			continue;
		}

		// if collide between immovable object skip generate contacts
		if ( ( candidate[i].m_body[0]->GetInverseMass() == 0 ) && ( candidate[i].m_body[1]->GetInverseMass() == 0 ) )
		{
			continue;
		}

		COLLISION_UTIL::DetectCollisionObjectAndObject( candidate[i].m_body[0]->GetGameObject(),
														candidate[i].m_body[0],
														candidate[i].m_body[1]->GetGameObject(),
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

void World::RunThinkGroup( ThinkingGroup group )
{
	float elapsedTime = GetTimer().GetElapsedTime();
	m_thinkTaskManager.RunThinkGroup( group, elapsedTime );
}

CPlayer* GetLocalPlayer( World& w )
{
	CPlayer* localPlayer = nullptr;

	const auto& gameObjects = w.GameObjects();
	for ( const auto& gameObject : gameObjects )
	{
		if ( CPlayer* player = Cast<CPlayer>( gameObject.get() ) )
		{
			localPlayer = player;
			break;
		}
	}

	return localPlayer;
}
