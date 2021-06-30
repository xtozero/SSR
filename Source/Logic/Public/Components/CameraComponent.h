#pragma once

#include "SceneComponent.h"
#include "Math/CXMFloat.h"

class CGameLogic;
class IRenderer;

namespace JSON
{
	class Value;
}

class CameraComponent : public SceneComponent
{
public:
	const CXMFLOAT3& GetForwardVector( ) const { return m_lookVector; }
	const CXMFLOAT3& GetRightVector( ) const { return m_rightVector; }
	const CXMFLOAT3& GetUpVector( ) const { return m_upVector; }

	void Move( const float right, const float up, const float look );
	void Move( CXMFLOAT3 delta );
	void Rotate( const float pitch, const float yaw, const float roll );
 
	const CXMFLOAT4X4& GetViewMatrix( ) const;
	const CXMFLOAT4X4& GetInvViewMatrix( ) const;
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
	mutable CXMFLOAT4X4 m_viewMatrix;
	mutable CXMFLOAT4X4 m_invViewMatrix;
	
	CXMFLOAT3 m_lookVector = { 0.f, 0.f, 1.f };
	CXMFLOAT3 m_upVector = { 0.f, 1.f, 0.f };
	CXMFLOAT3 m_rightVector = { 1.f, 0.f, 0.f };

	mutable bool m_needRecalc = false;

	bool m_enableRotate = true;
};

