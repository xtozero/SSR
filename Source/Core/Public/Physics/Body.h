#pragma once

#include "Math/Matrix.h"
#include "Math/Matrix3X3.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"

class RigidBody
{
public:
	void SetMass( float mass );
	float GetMass() const;

	void SetInverseMass( float inverseMass );
	float GetInverseMass() const;

	bool HasFiniteMass() const;

	void SetDamping( float linearDamping, float angularDamping );
	void SetLinearDamping( float linearDamping );
	float GetLinearDamping() const;
	void SetAngularDamping( float angularDamping );
	float GetAngularDamping() const;

	void SetPosition( const Point& position );
	Point GetPosition() const;

	void SetOrientation( const Quaternion& orientation );
	void SetOrientation( float pitch, float yaw, float roll );
	Quaternion GetOrientation() const;

	void SetVelocity( const Vector& velocity );
	Vector GetVelocity() const;
	void AddVelocity( const Vector& deltaVelocity );

	void SetRotation( const Vector& rotation );
	Vector GetRotation() const;
	void AddRotation( const Vector& deltaRotation );

	bool IsAwake() const { return m_isAwake; }
	void SetAwake( bool awake = true );

	bool IsCanSleep() const { return m_canSleep; }
	void SetCanSleep( bool canSleep = true );

	Matrix GetTransform() const;

	void SetInertiaTensor( const Matrix3X3& inertiaTensor );
	Matrix3X3 GetInertiaTensor() const;

	void SetInverseInertiaTensor( const Matrix3X3& inverseInertiaTensor );
	Matrix3X3 GetInverseInertiaTensor() const;

	Matrix3X3 GetInertiaTensorWorld() const;
	Matrix3X3 GetInverseInertiaTensorWorld() const;

	Point GetPointInWorldSpace( const Point& point );

	void AddForce( const Vector& force );
	void AddForceAtPoint( const Vector& force, const Point& point );
	void AddForceAtBodyPoint( const Vector& force, const Point& point );
	void AddTouque( const Vector& touque );

	void SetAcceleration( const Vector& acceleration );
	Vector GetAcceleration();

	Vector GetLastFrameAcceleration();

	void ClearAccumulators();

	bool Integrate( float duration );

	void CalculateDerivedData();

private:
	float m_inverseMass = 0.f;
	float m_linearDamping = 0.99f;
	float m_angularDamping = 0.8f;
	float m_motion = 0;
	float m_lastFrameMotion = 0;

	Point m_position;
	Quaternion m_orientation = Quaternion::Identity;
	Vector m_velocity;
	Vector m_rotation;

	Matrix m_transformMatrix;

	Matrix3X3 m_inverseInertiaTensor;
	Matrix3X3 m_inverseInertiaTenserWorld;

	Vector m_forceAccum;
	Vector m_torqueAccum;

	Vector m_acceleration;
	Vector m_lastFrameAcceleration;

	bool m_isAwake = true;
	bool m_canSleep = true;
};