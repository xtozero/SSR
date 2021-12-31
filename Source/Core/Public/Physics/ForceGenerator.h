#pragma once

#include "Math/Vector.h"
#include "Math/Matrix3X3.h"

#include <vector>

class RigidBody;

class ForceGenerator
{
public:
	virtual void UpdateForce( RigidBody* body, float duration ) = 0;
};

class Gravity : public ForceGenerator
{
public:
	explicit Gravity( const Vector& gravity ) : m_gravity( gravity ) {}
	virtual void UpdateForce( RigidBody* body, float duration ) override;

private:
	Vector m_gravity;
};

class Spring : public ForceGenerator
{
public:
	Spring( const Vector& localConnectionPt, RigidBody* other, const Vector& otherConnectionPt, float springConstant, float restLength ) :
		m_connectionPoint( localConnectionPt ), m_other( other ), m_otherConnectionPoint( otherConnectionPt ), m_springConstant( springConstant ), m_restLength( restLength ) {}
	virtual void UpdateForce( RigidBody* body, float duration ) override;
	
private:
	Vector m_connectionPoint;
	Vector m_otherConnectionPoint;
	RigidBody* m_other;

	float m_springConstant;
	float m_restLength;
};

class Aero : public ForceGenerator
{
public:
	Aero( const Matrix3X3& tensor, const Point position, const Vector& windSpeed ) :
		m_tensor( tensor ), m_position( position ), m_windSpeed( windSpeed ) {}
	virtual void UpdateForce( RigidBody* body, float duration ) override;

protected:
	void UpdateForceFromTensor( RigidBody* body, float duration, const Matrix3X3& tensor );
	Matrix3X3 m_tensor;

private:
	Point m_position;
	const Vector& m_windSpeed;
};

class AeroControl : public Aero
{
public:
	AeroControl( const Matrix3X3& base, const Matrix3X3& min, const Matrix3X3& max, const Vector& position, const Vector& windSpeed ) :
	Aero( base, position, windSpeed ), m_maxTensor( max ), m_minTensor( min ) {}
	virtual void UpdateForce( RigidBody* body, float duration ) override;
	void SetControl( float value ) { m_controlSetting = value; }

private:
	Matrix3X3 GetTensor( );

	Matrix3X3 m_maxTensor;
	Matrix3X3 m_minTensor;
	float m_controlSetting = 0;
};

class Buoyancy : public ForceGenerator
{
public:
	Buoyancy( const Point& cOfB, float maxDepth, float volume, float waterHeight, float liquidDensity = 1000.f ) :
		m_centreOfBuoyancy( cOfB ), m_maxDepth( maxDepth ), m_volume( volume ), m_waterHeight( waterHeight ), m_liquidDensity( liquidDensity ) {}
	virtual void UpdateForce( RigidBody* body, float duration ) override;

private:
	float m_maxDepth;
	float m_volume;
	float m_waterHeight;
	float m_liquidDensity;
	Point m_centreOfBuoyancy;
};

class ForceRegistry
{
public:
	void Add( RigidBody* body, ForceGenerator* fg );
	void Remove( RigidBody* body, ForceGenerator* fg );
	void Remove( RigidBody* body );

	void Clear( );

	void UpdateForces( float duration );

private:
	struct ForceRegistration
	{
		ForceRegistration( RigidBody* body, ForceGenerator* fg ) : m_body( body ), m_fg( fg ) {}

		RigidBody* m_body;
		ForceGenerator* m_fg;
	};

	using Registry = std::vector<ForceRegistration>;
	Registry m_registrations;
};