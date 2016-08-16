#pragma once

#include "IListener.h"
#include "Ray.h"

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

	bool IsContain( float x, float y )
	{
		return m_topLeftX <= x && m_topLeftY <= y && ( x - m_topLeftX ) <= m_width && ( y - m_topLeftY ) <= m_height;
	}
};

class CCamera;
class CGameObject;

class CPickingManager : public IListener
{
public:
	void PushViewport( const float topLeftX, const float topLeftY, const float width, const float height );
	void PushCamera( CCamera* camera );
	void PushInvProjection( float fov, float aspect, float zNear, float zFar, bool isLH = true );
	bool CreateWorldSpaceRay( CRay& ray, float x, float y );
	bool PickingObject( float x, float y );
	void ReleasePickingObject( );

	virtual void OnLButtonDown( const int x, const int y ) override;
	virtual void OnLButtonUp( const int x, const int y ) override;
	virtual void OnMouseMove( const int x, const int y ) override;

	using GameObjectsPtr = std::vector<std::shared_ptr<CGameObject>>*;
	explicit CPickingManager( const GameObjectsPtr objects );

private:
	std::vector<VIEWPORT> m_viewports;
	std::vector<CCamera*> m_cameras;
	std::vector<D3DXMATRIX> m_InvProjections;

	CGameObject* m_curSelectedObject;
	int m_curSelectedIdx;

	D3DXVECTOR2 m_prevMouseEventPos;

	float m_closestHitDist;

	GameObjectsPtr m_pGameObjects;
};

