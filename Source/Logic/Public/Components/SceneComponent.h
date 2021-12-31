#pragma once

#include "Component.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"
#include "Math/Vector.h"
#include "Physics/BoxSphereBounds.h"

class SceneComponent : public Component
{
public:
	using Component::Component;

	void SetPosition( const float x, const float y, const float z );
	void SetPosition( const Vector& pos );
	void SetScale( const float xScale, const float yScale, const float zScale );
	void SetRotate( const Quaternion& rotate );

	const Vector& GetPosition( ) const;
	const Vector& GetScale( ) const;
	const Quaternion& GetRotate( ) const;

	const Matrix& GetTransformMatrix( );
	const Matrix& GetInvTransformMatrix( );

	virtual BoxSphereBounds CalcBounds( const Matrix& transform );
	void UpdateBounds( );
	const BoxSphereBounds& Bounds( ) const
	{
		return m_bounds;
	}

private:
	void RebuildTransform( );

	Vector m_vecPos = Vector::ZeroVector;
	Vector m_vecScale = Vector::OneVector;
	Quaternion m_vecRotate = Quaternion::Identity;

	Matrix m_matTransform;
	Matrix m_invMatTransform;

	BoxSphereBounds m_bounds;

	bool m_needRebuildTransform = true;
};