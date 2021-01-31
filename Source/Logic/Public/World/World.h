#pragma once

#include "GameObject/GameObject.h"
#include "Math/CXMFloat.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollideBroad.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Contacts.h"
#include "Physics/ForceGenerator.h"
#include "Scene/INotifyGraphicsDevice.h"

#include <cstddef>
#include <memory>
#include <vector>

class CDebugOverlayManager;
class CPlayer;
class IScene;

class World : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	void Initialize( );
	void CleanUp( );

	void PreparePhysics( );
	void RunPhysics( float duration );

	void BeginFrame( );
	void RunFrame( float duration );
	void EndFrame( float duration );

	void SpawnObject( CGameLogic& gameLogic, Owner<CGameObject*> object );

	void UpdateObjectMovement( ObjectRelatedRigidBody* body, const BoundingSphere& volume );
	void DebugDrawBVH( CDebugOverlayManager& debugOverlay, unsigned int color, float duration );

	const std::vector<std::unique_ptr<CGameObject>>& GameObjects( )
	{
		return m_gameObjects;
	}

	IScene* Scene( ) const
	{
		return m_scene;
	}

private:
	int GenerateContacts( );
	void OnObjectSpawned( ObjectRelatedRigidBody* body, const BoundingSphere& volume );
	void OnObjectRemoved( ObjectRelatedRigidBody* body );

	std::vector<std::unique_ptr<CGameObject>> m_gameObjects;

	ForceRegistry m_registry;

	// Collision Acceleration
	BVHTree<BoundingSphere, ObjectRelatedRigidBody> m_bvhTree;

	static constexpr std::size_t MAX_CONTACTS = 256;
	Contact m_contacts[MAX_CONTACTS];
	ContactResolver m_resolver;
	CollisionData m_collisionData;

	Gravity m_gravity = Gravity( CXMFLOAT3( 0.f, -9.8f, 0.f ) );

	IScene* m_scene = nullptr;
};

CPlayer* GetLocalPlayer( World& w );
