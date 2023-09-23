#include "Physics/BodySetup.h"

#include "Physics/BodyInstance.h"
#include "Physics/PhysicsScene.h"

namespace logic
{
	const Vector& SphereElem::GetCenter() const
	{
		return m_center;
	}

	void SphereElem::SetCenter( const Vector& center )
	{
		m_center = center;
	}

	float SphereElem::GetRadius() const
	{
		return m_radius;
	}

	void SphereElem::SetRadius( float radius )
	{
		m_radius = radius;
	}

	SphereElem SphereElem::GetScaled( const Vector& scale3D ) const
	{
		SphereElem scaledSphere;

		float maxScale = std::max( { scale3D.x, scale3D.y, scale3D.z } );

		scaledSphere.m_center = m_center * scale3D;
		scaledSphere.m_radius = m_radius * maxScale;

		return scaledSphere;
	}

	const Vector& BoxElem::GetCenter() const
	{
		return m_center;
	}

	void BoxElem::SetCenter( const Vector& center )
	{
		m_center = center;
	}

	const Quaternion& BoxElem::GetRotation() const
	{
		return m_rotation;
	}

	void BoxElem::SetRotation( const Quaternion& rotation )
	{
		m_rotation = rotation;
	}

	const Vector& BoxElem::GetHalfSize() const
	{
		return m_halfSize;
	}

	void BoxElem::SetHalfSize( const Vector& halfSize )
	{
		m_halfSize = halfSize;
	}

	BoxElem BoxElem::GetScaled( const Vector& scale3D ) const
	{
		BoxElem scaledBox;

		scaledBox.m_center = m_center * scale3D;
		scaledBox.m_rotation = m_rotation;
		scaledBox.m_halfSize = m_halfSize * scale3D;

		return scaledBox;
	}

	std::vector<SphereElem>& BodySetup::SphereElems()
	{
		return m_geometries.m_sphereElems;
	}

	std::vector<BoxElem>& BodySetup::BoxElems()
	{
		return m_geometries.m_boxElems;
	}

	void BodySetup::AddGeometriesToPhysicsBody( BodyInstance& owningInstance ) const
	{
		if ( owningInstance.IsValid() == false )
		{
			return;
		}

		AddCollider( owningInstance.GetPhysicsHandle(), owningInstance.GetScale3D(), m_geometries );
	}
}
