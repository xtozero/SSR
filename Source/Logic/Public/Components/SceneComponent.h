#pragma once

#include "Component.h"
#include "Math/CXMFloat.h"

class SceneComponent : public Component
{
public:
	using Component::Component;

	void SetPosition( const float x, const float y, const float z );
	void SetPosition( const CXMFLOAT3& pos );
	void SetScale( const float xScale, const float yScale, const float zScale );
	void SetRotate( const CXMFLOAT4& rotate );
	void SetRotate( const float pitch, const float yaw, const float roll );
	void SetRotate( const CXMFLOAT3& pitchYawRoll );

	const CXMFLOAT3& GetPosition( ) const;
	const CXMFLOAT3& GetScale( ) const;
	const CXMFLOAT4& GetRotate( ) const;

	const CXMFLOAT4X4& GetTransformMatrix( );
	const CXMFLOAT4X4& GetInvTransformMatrix( );

private:
	void RebuildTransform( );

	CXMFLOAT3 m_vecPos = { 0.f, 0.f, 0.f };
	CXMFLOAT3 m_vecScale = { 1.f, 1.f, 1.f };
	CXMFLOAT4 m_vecRotate = { 0.f, 0.f, 0.f, 1.f };

	CXMFLOAT4X4 m_matTransform;
	CXMFLOAT4X4 m_invMatTransform;

	bool m_needRebuildTransform = true;
};