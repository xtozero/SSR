#pragma once
#include "BoundingSphere.h"
#include "CollideBroad.h"
#include "ForceGenerator.h"
#include "Math/CXMFloat.h"

#include <memory>
#include <vector>

class RigidBody;

class World
{
public:
	void StartFrame( );
	void RunPhysics( float duration );

private:
	void Integrate( float duration );

	using RigidBodies = std::vector<RigidBody*>;
	RigidBodies m_bodies;

	ForceRegistry m_registry;

	BVHNode<BoundingSphere> m_bvhRoot;
};