#pragma once

#include "Core/IListener.h"
#include "Math/CXMFloat.h"
#include "Physics/Ray.h"
#include "SizedTypes.h"

#include <algorithm>
#include <memory>
#include <vector>

struct VIEWPORT
{
	VIEWPORT( float topLeftX, float topLeftY, float width, float height ) :
	m_topLeftX( topLeftX ),
	m_topLeftY( topLeftY ),
	m_width( width ),
	m_height( height ){}

	float m_topLeftX;
	float m_topLeftY;
	float m_width;
	float m_height;

	bool IsContain( float x, float y ) const
	{
		return m_topLeftX <= x && m_topLeftY <= y && ( x - m_topLeftX ) <= m_width && ( y - m_topLeftY ) <= m_height;
	}
};

class CameraComponent;
class CGameObject;
struct UserInput;

class CPickingManager : public IListener
{
public:
	virtual void ProcessInput( const UserInput&, CGameLogic& gameLogic ) override;

	void PushViewport( const float topLeftX, const float topLeftY, const float width, const float height );
	void PopViewport( );
	void PushCamera( CameraComponent* camera );
	void PushInvProjection( float fov, float aspect, float zNear, float zFar, bool isLH = true );
	void PopInvProjection( );
	bool CreateWorldSpaceRay( CRay& ray, float x, float y );
	bool PickingObject( float x, float y );
	void ReleasePickingObject( );

	using GameObjectsPtr = std::vector<std::unique_ptr<CGameObject>>*;
	explicit CPickingManager( const GameObjectsPtr objects );

private:
	void OnMouseLButton( const UserInput& input );
	void OnMouseMove( const UserInput& input );

	std::vector<VIEWPORT> m_viewports;
	std::vector<CameraComponent*> m_cameras;
	std::vector<CXMFLOAT4X4> m_InvProjections;

	CGameObject* m_curSelectedObject = nullptr;
	int32 m_curSelectedIdx = -1;

	CXMFLOAT2 m_curMousePos;

	float m_closestHitDist = FLT_MAX;

	GameObjectsPtr m_pGameObjects;
};

