#pragma once

#include "Math/Quaternion.h"
#include "Math/Vector.h"
#include "SizedTypes.h"

#include <vector>

class BodyInstance;

class SphereElem
{
public:
	const Vector& GetCenter() const;
	void SetCenter( const Vector& center );
	
	float GetRadius() const;
	void SetRadius( float radius );

	SphereElem GetScaled( const Vector& scale3D ) const;

	SphereElem() = default;
	SphereElem( const Vector& center, float radius )
		: m_center( center )
		, m_radius( radius )
	{}

private:
	Vector m_center = Vector::ZeroVector;
	float m_radius = 0.5f;
};

class BoxElem
{
public:
	const Vector& GetCenter() const;
	void SetCenter( const Vector& center );

	const Quaternion& GetRotation() const;
	void SetRotation( const Quaternion& rotation );

	const Vector& GetHalfSize() const;
	void SetHalfSize( const Vector& halfSize );

	BoxElem GetScaled( const Vector& scale3D ) const;

	BoxElem() = default;
	BoxElem( const Vector& center, const Quaternion& rotation, const Vector& halfSize )
		: m_center( center )
		, m_rotation( rotation )
		, m_halfSize( halfSize )
	{}

private:
	Vector m_center = Vector::ZeroVector;
	Quaternion m_rotation = Quaternion::Identity;
	Vector m_halfSize = Vector( 0.5f, 0.5f, 0.5f );
};

struct AggregateGeom
{
	std::vector<SphereElem> m_sphereElems;
	std::vector<BoxElem> m_boxElems;

	size_t Size() const
	{
		return m_sphereElems.size() + m_boxElems.size();
	}
};

class BodySetup
{
public:
	template <typename T>
	void AddShapeToGeometry( const T& shape )
	{
		if constexpr ( std::is_same_v<T, SphereElem> )
		{
			m_geometries.m_sphereElems.push_back( shape );
		}
		else if constexpr ( std::is_same_v<T, BoxElem> )
		{
			m_geometries.m_boxElems.push_back( shape );
		}
	}

	std::vector<SphereElem>& SphereElems();
	std::vector<BoxElem>& BoxElems();

	void AddGeometriesToPhysicsBody( BodyInstance& owningInstance ) const;

private:
	AggregateGeom m_geometries;
};