#include "stdafx.h"

#include "Camera.h"
#include "CollisionUtil.h"
#include "../Engine/ConVar.h"
#include "GameObject.h"
#include "PickingManager.h"
#include "../Shared/Util.h"

extern std::vector<std::shared_ptr<CGameObject>> g_gameObjects;

namespace
{
	void WindowSpace2NDCSpace( D3DXVECTOR3& pos, const VIEWPORT& veiwport )
	{
		//뷰포트 기준으로 변환
		pos.x -= veiwport.m_topLeftX;
		pos.y -= veiwport.m_topLeftY;

		pos.x /= veiwport.m_width;
		pos.y /= veiwport.m_height;

		pos.x = ( pos.x * 2.f ) - 1.f;
		pos.y = ( pos.y * -2.f ) + 1.f;
	}

	ConVar( picked_object_name, "0", "flag show picked Object Name : 0 or 1" );
}

void CPickingManager::PushViewport( const float topLeftX, const float topLeftY, const float width, const float height )
{
	m_viewports.emplace_back( topLeftX, topLeftY, width, height );
}

void CPickingManager::PushCamera( CCamera* camera )
{
	m_cameras.push_back( camera );
}

void CPickingManager::PushInvProjection( float fov, float aspect, float zNear, float zFar, bool isLH )
{
	D3DXMATRIX projection;

	if ( isLH )
	{
		D3DXMatrixPerspectiveFovLH( &projection, fov, aspect, zNear, zFar );
	}
	else
	{
		D3DXMatrixPerspectiveFovRH( &projection, fov, aspect, zNear, zFar );
	}

	if ( D3DXMatrixInverse( &projection, nullptr, &projection ) )
	{
		m_InvProjections.push_back( projection );
	}
}

bool CPickingManager::CreateWorldSpaceRay( CRay& ray, float x, float y )
{
	if ( m_viewports.size( ) == 0 ||
		m_cameras.size( ) == 0 ||
		m_InvProjections.size( ) == 0 )
	{
		return false;
	}

	if ( m_viewports.size( ) != m_InvProjections.size( ) ||
		m_viewports.size( ) != m_cameras.size( ) ||
		m_cameras.size( ) != m_InvProjections.size( ) )
	{
		return false;
	}

	std::vector<VIEWPORT>::iterator curViewport = m_viewports.end();

	FOR_EACH_VEC( m_viewports, i )
	{
		if ( i->IsContain( x, y ) )
		{
			curViewport = i;
			break;
		}
	}

	if ( curViewport != m_viewports.end( ) )
	{
		m_curSelectedIdx = std::distance( m_viewports.begin( ), curViewport );

		if ( m_curSelectedIdx > -1 )
		{
			if ( CCamera* curSelectedCamera = m_cameras.at( m_curSelectedIdx ) )
			{
				const D3DXVECTOR3& origin = curSelectedCamera->GetOrigin( );
				D3DXVECTOR3 pos( x, y, 1.f );

				//NDC공간으로 변환
				WindowSpace2NDCSpace( pos, *curViewport );

				//카메라 공간으로 변환
				D3DXMATRIX& curInvProjection = m_InvProjections.at( m_curSelectedIdx );
				D3DXVec3TransformCoord( &pos, &pos, &curInvProjection );
				//월드 공간으로 변환
				D3DXVec3TransformCoord( &pos, &pos, &curSelectedCamera->GetInvViewMatrix( ) );

				D3DXVECTOR3& dir = pos - origin;
				D3DXVec3Normalize( &dir, &dir );

				ray.SetOrigin( origin );
				ray.SetDir( dir );

				return true;
			}
		}
	}

	return false;
}

bool CPickingManager::PickingObject( float x, float y, std::vector<std::shared_ptr<CGameObject>>& objects )
{
	CRay ray;
	if ( CreateWorldSpaceRay( ray, x, y ) )
	{
		m_curSelectedObject = nullptr;
		m_closestHitDist = FLT_MAX;

		FOR_EACH_VEC( objects, i )
		{
			const CGameObject* object = i->get( );

			if ( object == nullptr || object->IgnorePicking() )
			{
				continue;
			}

			float hitDist = COLLISION_UTIL::IntersectWithRay( *object, ray, RIGID_BODY_TYPE::AABB );

			if ( hitDist >= 0 && m_closestHitDist > hitDist )
			{
				m_closestHitDist = hitDist;
				m_curSelectedObject = i->get( );
			}
		}

		if ( m_curSelectedObject )
		{
			if ( picked_object_name.GetBool( ) )
			{
				DebugMsg( "Object Selected - %s\n", m_curSelectedObject->GetName( ).c_str( ) );
			}
			return true;
		}
	}

	return false;
}

void CPickingManager::ReleasePickingObject( )
{
	if ( m_curSelectedObject )
	{
		m_curSelectedObject->SetPicked( false );
	}

	m_curSelectedObject = nullptr;

	if ( m_curSelectedIdx > -1 )
	{
		if ( CCamera* curCamera = m_cameras.at( m_curSelectedIdx ) )
		{
			curCamera->SetEnableRotate( true );
		}
	}
}

void CPickingManager::OnLButtonDown( const int x, const int y )
{
	if ( PickingObject( static_cast<float>( x ), static_cast<float>( y ), g_gameObjects ) )
	{
		if ( m_curSelectedObject )
		{
			m_curSelectedObject->SetPicked( true );
		}

		if ( m_curSelectedIdx > -1 )
		{
			if ( CCamera* curCamera = m_cameras.at( m_curSelectedIdx ) )
			{
				curCamera->SetEnableRotate( false );
			}
		}
	}
}

void CPickingManager::OnLButtonUp( const int x, const int y )
{
	ReleasePickingObject( );
}

void CPickingManager::OnMouseMove( const int x, const int y )
{
	float xPos = static_cast<float>( x );
	float yPos = static_cast<float>( y );

	D3DXVECTOR2 curPos = D3DXVECTOR2( xPos, yPos );
	D3DXVECTOR2 delta = curPos - m_prevMouseEventPos;

	if ( m_curSelectedObject && m_curSelectedIdx > -1 )
	{
		VIEWPORT& curViewport = m_viewports.at( m_curSelectedIdx );

		if ( !curViewport.IsContain( static_cast<float>( x ), static_cast<float>( y ) ) )
		{
			//뷰포트를 벗어 났으면 픽킹 해제
			ReleasePickingObject( );
		}
		else if ( CCamera* curCamera = m_cameras.at( m_curSelectedIdx ) )
		{
			D3DXVECTOR3 curPos( static_cast<float>( x ), static_cast<float>( y ), 1.f );
			D3DXVECTOR3 prevPos( m_prevMouseEventPos.x, m_prevMouseEventPos.y, 1.f );

			//NDC공간으로 변환
			WindowSpace2NDCSpace( curPos, curViewport );
			WindowSpace2NDCSpace( prevPos, curViewport );

			//카메라 공간으로 변환
			D3DXMATRIX& curInvProjection = m_InvProjections.at( m_curSelectedIdx );
			D3DXVec3TransformCoord( &curPos, &curPos, &curInvProjection );
			D3DXVec3TransformCoord( &prevPos, &prevPos, &curInvProjection );
			
			//월드 공간으로 변환
			D3DXVec3TransformCoord( &curPos, &curPos, &curCamera->GetInvViewMatrix( ) );
			D3DXVec3TransformCoord( &prevPos, &prevPos, &curCamera->GetInvViewMatrix( ) );

			//삼각형 닮음을 이용한 월드 공간 이동 벡터 계산
			D3DXVECTOR3& rayDir = prevPos - curCamera->GetOrigin( );
			float farPlaneRayDist = D3DXVec3Length( &rayDir );
			D3DXVec3Normalize( &rayDir, &rayDir );

			CRay pickingRay( curCamera->GetOrigin(), rayDir );
			float hitDist = COLLISION_UTIL::IntersectWithRay( *m_curSelectedObject, pickingRay, RIGID_BODY_TYPE::AABB );

			if ( hitDist >= 0.f )
			{
				D3DXVECTOR3& dir = curPos - prevPos;
				D3DXVECTOR3 curPosition = m_curSelectedObject->GetPosition( );

				curPosition += ( dir * hitDist ) / farPlaneRayDist;
				m_curSelectedObject->SetPosition( curPosition );
			}
			else
			{
				ReleasePickingObject( );
			}
		}
	}

	m_prevMouseEventPos = curPos;
}

CPickingManager::CPickingManager( )
: m_curSelectedObject( nullptr ),
m_curSelectedIdx( -1 ),
m_closestHitDist( FLT_MAX )
{
}


CPickingManager::~CPickingManager( )
{
}
