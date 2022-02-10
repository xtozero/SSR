#pragma once

#include "SceneComponent.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

class CGameLogic;
class IRenderer;

namespace JSON
{
	class Value;
}

class CameraComponent : public SceneComponent
{
	GENERATE_CLASS_TYPE_INFO( CameraComponent )

public:
	const Vector& GetForwardVector( ) const { return m_lookVector; }
	const Vector& GetRightVector( ) const { return m_rightVector; }
	const Vector& GetUpVector( ) const { return m_upVector; }

	void Move( const float right, const float up, const float look );
	void Move( Vector delta );
	void Rotate( const float pitch, const float yaw, const float roll );
 
	const Matrix& GetViewMatrix( ) const;
	const Matrix& GetInvViewMatrix( ) const;
	void SetEnableRotate( bool isEnable ) { m_enableRotate = isEnable; }

	void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json );

	explicit CameraComponent( CGameObject* pOwner );

private:
	void ReCalcViewMatrix( ) const;
	void MarkCameraTransformDirty( )
	{
		m_needRecalc = true;
	}

private:
	mutable Matrix m_viewMatrix;
	mutable Matrix m_invViewMatrix;
	
	Vector m_lookVector = Vector::ForwardVector;
	Vector m_upVector = Vector::UpVector;
	Vector m_rightVector = Vector::RightVector;

	mutable bool m_needRecalc = false;

	bool m_enableRotate = true;
};

