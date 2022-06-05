#include "stdafx.h"
#include "GameObject/PickingManager.h"

#include "Components/CameraComponent.h"
#include "ConsoleMessage/ConVar.h"
#include "DebugUtil.h"
#include "GameObject/GameObject.h"
#include "Math/TransformationMatrix.h"
#include "Physics/CollisionUtil.h"
#include "UserInput/UserInput.h"

using namespace DirectX;

namespace
{
	void WindowSpace2NDCSpace( Vector& pos, const VIEWPORT& veiwport )
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

void CPickingManager::ProcessInput( const UserInput& input, CGameLogic& /*gameLogic*/ )
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

void CPickingManager::PopViewport( )
{
	m_viewports.pop_back( );
}

void CPickingManager::PushCamera( CameraComponent* camera )
{
	m_cameras.push_back( camera );
}

void CPickingManager::PushInvProjection( float fov, float aspect, float zNear, float zFar, bool isLH )
{
	Matrix projection;

	if ( isLH )
	{
		projection = PerspectiveMatrix( fov, aspect, zNear, zFar );
	}
	else
	{
		projection = PerspectiveMatrix( fov, aspect, zNear, zFar );
	}

	m_InvProjections.emplace_back( projection.Inverse() );
}

void CPickingManager::PopInvProjection( )
{
	m_InvProjections.pop_back( );
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

	for ( auto viewport = m_viewports.begin(); viewport != m_viewports.end(); ++viewport )
	{
		if ( viewport->IsContain( x, y ) )
		{
			curViewport = viewport;
			break;
		}
	}

	if ( curViewport != m_viewports.end( ) )
	{
		m_curSelectedIdx = static_cast<int32>( std::distance( m_viewports.begin( ), curViewport ) );

		if ( m_curSelectedIdx > -1 )
		{
			if ( CameraComponent* curSelectedCamera = m_cameras.at( m_curSelectedIdx ) )
			{
				const Vector& origin = curSelectedCamera->GetPosition( );
				Vector pos( x, y, 1.f );

				//NDC공간으로 변환
				WindowSpace2NDCSpace( pos, *curViewport );

				//카메라 공간으로 변환
				const Matrix& curInvProjection = m_InvProjections.at( m_curSelectedIdx );
				pos = curInvProjection.TransformPosition( pos );
				//월드 공간으로 변환
				pos = curSelectedCamera->GetInvViewMatrix().TransformPosition( pos );

				Vector dir = pos - origin;
				dir = dir.GetNormalized( );

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

	//CRay ray;
	//if ( CreateWorldSpaceRay( ray, x, y ) )
	//{
	//	m_curSelectedObject = nullptr;
	//	m_closestHitDist = FLT_MAX;

	//	for ( auto& object : *m_pGameObjects )
	//	{
	//		if ( object == nullptr || object->IgnorePicking() )
	//		{
	//			continue;
	//		}

	//		if ( const ICollider* pCollider = object->GetCollider( COLLIDER::AABB ) )
	//		{
	//			float hitDist = pCollider->Intersect( ray );

	//			if ( hitDist >= 0 && m_closestHitDist > hitDist )
	//			{
	//				m_closestHitDist = hitDist;
	//				m_curSelectedObject = object.get( );
	//			}
	//		}
	//	}

	//	if ( m_curSelectedObject )
	//	{
	//		if ( picked_object_name.GetBool( ) )
	//		{
	//			DebugMsg( "Object Selected - %s\n", m_curSelectedObject->GetName( ).c_str( ) );
	//		}
	//		return true;
	//	}
	//}

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
		if ( CameraComponent* curCamera = m_cameras.at( m_curSelectedIdx ) )
		{
			curCamera->SetEnableRotation( true );
		}
	}
}

CPickingManager::CPickingManager( const GameObjectsPtr objects ) :
	m_pGameObjects( objects )
{
}

void CPickingManager::OnMouseLButton( const UserInput& input )
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
			if ( CameraComponent* curCamera = m_cameras.at( m_curSelectedIdx ) )
			{
				curCamera->SetEnableRotation( false );
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
	//Vector2 prevMousePos = m_curMousePos;
	//m_curMousePos += { input.m_axis[UserInput::X_AXIS], input.m_axis[UserInput::Y_AXIS] };

	//if ( m_curSelectedObject && m_curSelectedIdx > -1 )
	//{
	//	VIEWPORT& curViewport = m_viewports.at( m_curSelectedIdx );

	//	if ( !curViewport.IsContain( m_curMousePos.x, m_curMousePos.y ) )
	//	{
	//		//뷰포트를 벗어 났으면 픽킹 해제
	//		ReleasePickingObject( );
	//	}
	//	else if ( CameraComponent* curCamera = m_cameras.at( m_curSelectedIdx ) )
	//	{
	//		Vector curPos( m_curMousePos.x, m_curMousePos.y, 1.f );
	//		Vector prevPos( prevMousePos.x, prevMousePos.y, 1.f );

	//		//NDC공간으로 변환
	//		WindowSpace2NDCSpace( curPos, curViewport );
	//		WindowSpace2NDCSpace( prevPos, curViewport );

	//		//카메라 공간으로 변환
	//		Matrix& curInvProjection = m_InvProjections.at( m_curSelectedIdx );
	//		curPos = curInvProjection.TransformPosition( curPos );
	//		prevPos = curInvProjection.TransformPosition( prevPos );

	//		//월드 공간으로 변환
	//		curPos = curCamera->GetInvViewMatrix().TransformPosition( curPos );
	//		prevPos = curCamera->GetInvViewMatrix().TransformPosition( prevPos );

	//		//삼각형 닮음을 이용한 월드 공간 이동 벡터 계산
	//		const Vector& origin = curCamera->GetPosition( );
	//		auto rayDir = prevPos - origin;
	//		float farPlaneRayDist = rayDir.Length();
	//		rayDir = rayDir.GetNormalized();

	//		CRay pickingRay( origin, rayDir );
	//		float hitDist = m_curSelectedObject->GetCollider( COLLIDER::AABB )->Intersect( pickingRay );

	//		if ( hitDist >= 0.f )
	//		{
	//			Vector curPosition = m_curSelectedObject->GetPosition( );

	//			curPosition += ( ( curPos - prevPos ) * hitDist ) / farPlaneRayDist;
	//			m_curSelectedObject->SetPosition( curPosition );
	//		}
	//		else
	//		{
	//			ReleasePickingObject( );
	//		}
	//	}
	//}
}