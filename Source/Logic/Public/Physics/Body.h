#pragma once

#include "Math/CXMFloat.h"

class RigidBody
{
public:
	void SetMass( float mass );
	float GetMass( ) const;

	void SetInverseMass( float inverseMass );
	float GetInverseMass( ) const;

	bool HasFiniteMass() const;

	void SetDamping( float linearDamping, float angularDamping );
	void SetLinearDamping( float linearDamping );
	float GetLinearDamping( ) const;
	void SetAngularDamping( float angularDamping );
	float GetAngularDamping( ) const;

	void SetPosition( const CXMFLOAT3& position );
	CXMFLOAT3 GetPosition( ) const;
	
	void SetOrientation( const CXMFLOAT4& orientation );
	void SetOrientation( float pitch, float yaw, float roll );
	CXMFLOAT4 GetOrientation( ) const;
	
	void SetVelocity( const CXMFLOAT3& velocity );
	CXMFLOAT3 GetVelocity( ) const;
	void AddVelocity( const CXMFLOAT3& deltaVelocity );
	
	void SetRotation( const CXMFLOAT3& rotation );
	CXMFLOAT3 GetRotation( ) const;
	void AddRotation( const CXMFLOAT3& deltaRotation );
	
	bool IsAwake( ) const { return m_isAwake; }
	void SetAwake( bool awake = true );

	bool IsCanSleep( ) const { return m_canSleep; }
	void SetCanSleep( bool canSleep = true );

	CXMFLOAT4X4 GetTransform( ) const;

	void SetInertiaTensor( const CXMFLOAT3X3& inertiaTensor );
	CXMFLOAT3X3 GetInertiaTensor( ) const;

	void SetInverseInertiaTensor( const CXMFLOAT3X3& inverseInertiaTensor );
	CXMFLOAT3X3 GetInverseInertiaTensor( ) const;

	CXMFLOAT3X3 GetInertiaTensorWorld( ) const;
	CXMFLOAT3X3 GetInverseInertiaTensorWorld( ) const;

	CXMFLOAT3 GetPointInWorldSpace( const CXMFLOAT3& point );

	void AddForce( const CXMFLOAT3& force );
	void AddForceAtPoint( const CXMFLOAT3& force, const CXMFLOAT3& point );
	void AddForceAtBodyPoint( const CXMFLOAT3& force, const CXMFLOAT3& point );
	void AddTouque( const CXMFLOAT3& touque );

	void SetAcceleration( const CXMFLOAT3& acceleration );
	CXMFLOAT3 GetAcceleration( );

	CXMFLOAT3 GetLastFrameAcceleration( );

	void ClearAccumulators( );

	bool Integrate( float duration );

	void CalculateDerivedData( );

private:
	float m_inverseMass = 0.f;
	float m_linearDamping = 0.99f;
	float m_angularDamping = 0.8f;
	float m_motion = 0;
	float m_lastFrameMotion = 0;
	
	CXMFLOAT3 m_position;
	CXMFLOAT4 m_orientation = CXMFLOAT4( 0.f, 0.f, 0.f, 1.f );
	CXMFLOAT3 m_velocity;
	CXMFLOAT3 m_rotation;
	
	CXMFLOAT4X4 m_transformMatrix;

	CXMFLOAT3X3 m_inverseInertiaTensor;
	CXMFLOAT3X3 m_inverseInertiaTenserWorld;

	CXMFLOAT3 m_forceAccum;
	CXMFLOAT3 m_torqueAccum;

	CXMFLOAT3 m_acceleration;
	CXMFLOAT3 m_lastFrameAcceleration;

	bool m_isAwake = true;
	bool m_canSleep = true;
};