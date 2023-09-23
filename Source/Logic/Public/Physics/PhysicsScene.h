#pragma once

#include "Physics/Body.h"
#include "Physics/BoundingSphere.h"
#include "Physics/BoxSphereBounds.h"
#include "Physics/CollideBroad.h"
#include "Physics/CollideNarrow.h"
#include "Physics/Contacts.h"
#include "Physics/ForceGenerator.h"
#include "SizedTypes.h"
#include "SparseArray.h"

#include <compare>
#include <map>
#include <memory>
#include <vector>

class Transform;

namespace logic
{
	class PhysicsScene;
	class PrimitiveComponent;

	struct AggregateGeom;

	struct PhysicsHandle
	{
		size_t m_id;
		PhysicsScene* m_scene;

		std::strong_ordering operator<=>( const PhysicsHandle& other ) const = default;
	};

	class PhysicsBody : public RigidBody
	{
	public:
		BoxSphereBounds Bounds() const;

		const ICollider* GetCollider();
		void AddCollider( std::vector<std::unique_ptr<ICollider>>& colliders );

		int32 GetDirty() const
		{
			return m_dirtyFlag;
		}

		void SetDirty( int32 dirtyFlag )
		{
			m_dirtyFlag |= dirtyFlag;
		}

		void ResetDirty()
		{
			m_dirtyFlag = 0;
		}

		PrimitiveComponent* GetOwnerComponent()
		{
			return m_ownerComponent;
		}

		void SetOwerComponent( PrimitiveComponent* ownerComponent )
		{
			m_ownerComponent = ownerComponent;
		}

	private:
		std::unique_ptr<ICollider> m_collider;
		BoxSphereBounds m_bounds;
		int32 m_dirtyFlag = 0;
		PrimitiveComponent* m_ownerComponent = nullptr;
	};

	class SceneForceGenerator
	{
	public:
		void Add( const PhysicsHandle& handle, ForceGenerator* fg );
		void Remove( const PhysicsHandle& handle );

		void Clear();

		void GenerateForces( float duration );

	private:
		std::map<ForceGenerator*, SparseArray<PhysicsHandle>> m_entries;
	};

	class PhysicsScene
	{
	public:
		void BeginFrame( float elapsedTime );
		void EndFrame();

		PhysicsHandle CreatePhysicBody( PrimitiveComponent& ownerComponent, const Transform& bodyTransform );
		void DeletePhysicsBody( const PhysicsHandle& handle );

		PhysicsBody& GetPhysicsBody( const PhysicsHandle& handle );

	private:
		void PreparePhysics();
		void RunPhysics( float elapsedTime );
		int32 GenerateContacts();

		SceneForceGenerator m_forceGenerator;

		// Collision Acceleration
		BVHTree<BoxSphereBounds, PhysicsBody> m_bvhTree;

		static constexpr uint32 MAX_CONTACTS = 256;
		Contact m_contacts[MAX_CONTACTS];
		ContactResolver m_resolver;
		CollisionData m_collisionData;

		Gravity m_gravity = Gravity( Vector( 0.f, -9.8f, 0.f ) );

		float m_remainingSimulateTime = 0.f;

		SparseArray<PhysicsBody> m_bodies;
	};

	void AddCollider( const PhysicsHandle& handle, const Vector& scale3D, const AggregateGeom& geometries );
}
