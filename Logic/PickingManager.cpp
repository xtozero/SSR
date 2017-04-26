#include "stdafx.h"

#include "Camera.h"
#include "CollisionUtil.h"
#include "GameObject.h"
#include "PickingManager.h"

#include "../Engine/ConVar.h"
#include "../shared/UserInput.h"
#include "../Shared/Util.h"

using namespace DirectX;

namespace
{
	void WindowSpace2NDCSpace( CXMFLOAT3& pos, const VIEWPORT& veiwport )
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

void CPickingManager::ProcessInput( const UserInput& input )
{
	switch ( input.m_code )
	{
	case UIC_MOUSE_LEFT:
		OnMouseLButton( input );
		break;
	case UIC_MOUSE_MOVE:
		OnMouseMove( input );
		break;
	default:
		break;
	}
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
	CXMFLOAT4X4 projection;

	if ( isLH )
	{
		projection = XMMatrixPerspectiveFovLH( fov, aspect, zNear, zFar );
	}
	else
	{
		projection = XMMatrixPerspectiveFovRH( fov, aspect, zNear, zFar );
	}

	m_InvProjections.emplace_back( XMMatrixInverse( nullptr, projection ) );
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

	for ( auto& viewport = m_viewports.begin(); viewport != m_viewports.end(); ++viewport )
	{
		if ( viewport->IsContain( x, y ) )
		{
			curViewport = viewport;
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
				const CXMFLOAT3& origin = curSelectedCamera->GetOrigin( );
				CXMFLOAT3 pos( x, y, 1.f );

				//NDC공간으로 변환
				WindowSpace2NDCSpace( pos, *curViewport );

				//카메라 공간으로 변환
				const CXMFLOAT4X4& curInvProjection = m_InvProjections.at( m_curSelectedIdx );
				pos = XMVector3TransformCoord( pos, curInvProjection );
				//월드 공간으로 변환
				pos = XMVector3TransformCoord( pos, curSelectedCamera->GetInvViewMatrix( ) );

				XMVECTOR dir = pos - origin;
				dir = XMVector3Normalize( dir );

				ray.SetOrigin( origin );
				ray.SetDir( dir );

				return true;
			}
		}
	}

	return false;
}

bool CPickingManager::PickingObject( float x, float y )
{
	assert( m_pGameObjects != nullptr );

	CRay ray;
	if ( CreateWorldSpaceRay( ray, x, y ) )
	{
		m_curSelectedObject = nullptr;
		m_closestHitDist = FLT_MAX;

		for ( auto& object : *m_pGameObjects )
		{
			if ( object == nullptr || object->IgnorePicking() )
			{
				continue;
			}

			float hitDist = COLLISION_UTIL::IntersectWithRay( *object, ray, RIGID_BODY_TYPE::AABB );

			if ( hitDist >= 0 && m_closestHitDist > hitDist )
			{
				m_closestHitDist = hitDist;
				m_curSelectedObject = object.get();
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

CPickingManager::CPickingManager( const GameObjectsPtr objects ) :
	m_pGameObjects( objects )
{
}

void CPickingManager::OnMouseLButton( const UserInput & input )
{
	m_curMousePos = { input.m_axis[UserInput::X_AXIS], input.m_axis[UserInput::Y_AXIS] };

	bool isPressed = input.m_axis[UserInput::Z_AXIS] < 0;
	if ( isPressed && PickingObject( m_curMousePos.x, m_curMousePos.y ) )
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
	else
	{
		ReleasePickingObject( );
	}
}

void CPickingManager::OnMouseMove( const UserInput& input )
{
	CXMFLOAT2 prevMousePos = m_curMousePos;
	m_curMousePos += { input.m_axis[UserInput::X_AXIS], input.m_axis[UserInput::Y_AXIS] };

	if ( m_curSelectedObject && m_curSelectedIdx > -1 )
	{
		VIEWPORT& curViewport = m_viewports.at( m_curSelectedIdx );

		if ( !curViewport.IsContain( m_curMousePos.x, m_curMousePos.y ) )
		{
			//뷰포트를 벗어 났으면 픽킹 해제
			ReleasePickingObject( );
		}
		else if ( CCamera* curCamera = m_cameras.at( m_curSelectedIdx ) )
		{
			CXMFLOAT3 curPos( m_curMousePos.x, m_curMousePos.y, 1.f );
			CXMFLOAT3 prevPos( prevMousePos.x, prevMousePos.y, 1.f );

			//NDC공간으로 변환
			WindowSpace2NDCSpace( curPos, curViewport );
			WindowSpace2NDCSpace( prevPos, curViewport );

			//카메라 공간으로 변환
			CXMFLOAT4X4& curInvProjection = m_InvProjections.at( m_curSelectedIdx );
			curPos = XMVector3TransformCoord( curPos, curInvProjection );
			prevPos = XMVector3TransformCoord( prevPos, curInvProjection );

			//월드 공간으로 변환
			curPos = XMVector3TransformCoord( curPos, curCamera->GetInvViewMatrix( ) );
			prevPos = XMVector3TransformCoord( prevPos, curCamera->GetInvViewMatrix( ) );

			//삼각형 닮음을 이용한 월드 공간 이동 벡터 계산
			const CXMFLOAT3& origin = curCamera->GetOrigin( );
			XMVECTOR rayDir = prevPos - origin;
			float farPlaneRayDist = XMVectorGetX( XMVector3Length( rayDir ) );
			rayDir = XMVector3Normalize( rayDir );

			CRay pickingRay( origin, rayDir );
			float hitDist = COLLISION_UTIL::IntersectWithRay( *m_curSelectedObject, pickingRay, RIGID_BODY_TYPE::AABB );

			if ( hitDist >= 0.f )
			{
				XMVECTOR curPosition = m_curSelectedObject->GetPosition( );

				curPosition += ( ( curPos - prevPos ) * hitDist ) / farPlaneRayDist;
				m_curSelectedObject->SetPosition( curPosition );
			}
			else
			{
				ReleasePickingObject( );
			}
		}
	}
}