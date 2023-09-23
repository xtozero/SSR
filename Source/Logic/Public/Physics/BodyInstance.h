#pragma once

#include "Math/Matrix.h"
#include "Math/Transform.h"

#include "Physics/PhysicsScene.h"

#include <optional>

class Transform;

namespace logic
{
	class BodySetup;
	class PrimitiveComponent;

	class BodyInstance
	{
	public:
		void InitBody( PrimitiveComponent& ownerComponent, const BodySetup& bodySetup, const Transform& bodyTransform, PhysicsScene& physicsScene );
		void TermBody();

		bool IsValid() const;
		PhysicsScene* GetPhysicsScene() const;

		void SetMass( float mass );

		void SetLinearDamping( float linearDamping );
		void SetAngularDamping( float angularDamping );

		Transform GetTransform() const;

		PhysicsHandle GetPhysicsHandle() const;

		const Vector& GetScale3D() const;

	private:
		PhysicsBody& GetPhysicsBody() const;
		void InitDynamicPropoerties();

		std::optional<PhysicsHandle> m_physicsHandle;

		float m_inverseMass = 0.f;
		float m_linearDamping = 0.99f;
		float m_angularDamping = 0.8f;

		Vector m_scale3D = Vector::OneVector;
	};
}
