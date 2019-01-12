#pragma once

#include "BoundingSphere.h"
#include "CollideBroad.h"
#include "CollideNarrow.h"
#include "Contacts.h"
#include "ForceGenerator.h"
#include "GameObject/GameObject.h"
#include "Math/CXMFloat.h"

#include <memory>
#include <vector>

class CDebugOverlayManager;

class World
{
public:
	void StartFrame( );
	void RunPhysics( float duration );

	void OnObjectSpawned( ObjectRelatedRigidBody* body, const BoundingSphere& volume );
	void UpdateObjectMovement( ObjectRelatedRigidBody* body, const BoundingSphere& volume );
	void DebugDrawBVH( CDebugOverlayManager& debugOverlay, unsigned int color, float duration );

private:
	int GenerateContacts( );

	ForceRegistry m_registry;

	// Collision Acceleration
	BVHTree<BoundingSphere, ObjectRelatedRigidBody> m_bvhTree;

	static constexpr int MAX_CONTACTS = 256;
	Contact m_contacts[MAX_CONTACTS];
	ContactResolver m_resolver;
	CollisionData m_collisionData;

	Gravity m_gravity = Gravity( CXMFLOAT3( 0.f, -9.8f, 0.f ) );
};