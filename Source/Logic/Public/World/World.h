#pragma once

#include "Core/Timer.h"
#include "GameObject/GameObject.h"
#include "Math/Vector.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollideBroad.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Contacts.h"
#include "Physics/ForceGenerator.h"
#include "Scene/INotifyGraphicsDevice.h"
#include "SizedTypes.h"

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

	void Initialize();
	void CleanUp();

	const Timer& GetTimer() const { return m_clock; }
	void Pause();
	void Resume();

	void PreparePhysics();
	void RunPhysics( float duration );

	void BeginFrame();
	void RunFrame();
	void EndFrame();

	void SpawnObject( CGameLogic& gameLogic, Owner<CGameObject*> object );

	void UpdateObjectMovement( ObjectRelatedRigidBody* body, const BoundingSphere& volume );
	void DebugDrawBVH( CDebugOverlayManager& debugOverlay, uint32 color, float duration );

	const std::vector<std::unique_ptr<CGameObject>>& GameObjects( )
	{
		return m_gameObjects;
	}

	IScene* Scene() const
	{
		return m_scene;
	}

private:
	int32 GenerateContacts();
	void OnObjectSpawned( ObjectRelatedRigidBody* body, const BoundingSphere& volume );
	void OnObjectRemoved( ObjectRelatedRigidBody* body );

	std::vector<std::unique_ptr<CGameObject>> m_gameObjects;

	ForceRegistry m_registry;

	// Collision Acceleration
	BVHTree<BoundingSphere, ObjectRelatedRigidBody> m_bvhTree;

	static constexpr uint32 MAX_CONTACTS = 256;
	Contact m_contacts[MAX_CONTACTS];
	ContactResolver m_resolver;
	CollisionData m_collisionData;

	Gravity m_gravity = Gravity( Vector( 0.f, -9.8f, 0.f ) );

	IScene* m_scene = nullptr;

	Timer m_clock;
};

CPlayer* GetLocalPlayer( World& w );
