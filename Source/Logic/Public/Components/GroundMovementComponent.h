#pragma once

#include "Component.h"
#include "Math/Vector.h"

#include <utility>

namespace logic
{
	class GroundMovementComponent : public Component
	{
		GENERATE_CLASS_TYPE_INFO( GroundMovementComponent )

	public:
		virtual void ThinkComponent( float elapsedTime ) override;

		virtual void LoadProperty( const json::Value& json ) override;

		void Update( const Vector& force, float elapsedTime );

		void SetFriction( const std::pair<float, float>& friction );
		void SetMaxForceMagnitude( float magnitude );

		GroundMovementComponent( CGameObject* pOwner, const char* name );

	protected:
		virtual bool ShouldCreateRenderState() const;
		virtual bool ShouldCreatePhysicsState() const;

	private:
		Vector GetDelta( float elapsedTime ) const;

		float m_mass = 1.f;
		Vector m_velocity = Vector::ZeroVector;

		float m_maxForceMag = FLT_MAX;
		std::pair<float, float> m_friction = {}; // static, kinetic
		float m_kineticForceScale = 1.f;

		bool m_isMoving = false;
	};
}
