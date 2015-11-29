#include "stdafx.h"
#include "GameObject.h"
#include "../RenderCore/Direct3D11.h"
#include "Timer.h"

extern IRenderer* gRenderer;

void CGameObject::SetPosition( const float x, const float y, const float z )
{
	m_vecPos = D3DXVECTOR3( x, y, z );
	m_needRebuildTransform = true;
}

void CGameObject::SetScale( const float xScale, const float yScale, const float zScale )
{
	m_vecScale = D3DXVECTOR3( xScale, yScale, zScale );
	m_needRebuildTransform = true;
}

inline void CGameObject::SetRotate( const float pitch, const float yaw, const float roll )
{
	m_vecRotate = D3DXVECTOR3( pitch, yaw, roll );
	m_needRebuildTransform = true;
}

inline const D3DXVECTOR3& CGameObject::GetPosition( )
{
	return m_vecPos;
}

inline const D3DXVECTOR3& CGameObject::GetScale( )
{
	return m_vecScale;
}

inline const D3DXVECTOR3& CGameObject::GetRotate( )
{
	return m_vecRotate;
}

const D3DXMATRIX& CGameObject::GetTransformMatrix( ) const
{
	return m_matTransform;
}

void CGameObject::LoadModelMesh( const TCHAR* pModelName )
{
	m_meshName = pModelName;

	m_pModel = gRenderer->GetModelPtr( pModelName );
}

void CGameObject::Render( )
{
	if ( ShouldDraw() && m_pModel )
	{
		if ( m_needRebuildTransform )
		{
			RebuildTransform( );
		}

		gRenderer->DrawModel( m_pModel );
	}
}

void CGameObject::Think( )
{
	//Test Code
	float fDeltaTime = CTimer::GetInstance( ).GetElapsedTIme( );
	const D3DXVECTOR3& curRotate = GetRotate( );

	SetRotate( 0.f, curRotate.y + D3DXToRadian( 90.f ) * fDeltaTime, 0.f );
}

CGameObject::CGameObject( ) :
m_vecPos( 0.f, 0.f, 0.f ),
m_vecScale( 1.f, 1.f, 1.f ),
m_vecRotate( 0.f, 0.f, 0.f ),
m_pModel( nullptr ),
m_needRebuildTransform( false )
{
	D3DXMatrixIdentity( &m_matTransform );
}

CGameObject::~CGameObject( )
{
}

void CGameObject::RebuildTransform( )
{
	//STR
	D3DXMATRIX scale;
	D3DXMATRIX rotate;

	D3DXMatrixScaling( &scale, m_vecScale.x, m_vecScale.y, m_vecScale.z );
	D3DXMatrixRotationYawPitchRoll( &rotate, m_vecRotate.y, m_vecRotate.x, m_vecRotate.z );

	D3DXMatrixMultiply( &m_matTransform, &scale, &rotate );

	m_matTransform._41 = m_vecPos.x;
	m_matTransform._42 = m_vecPos.y;
	m_matTransform._43 = m_vecPos.z;

	m_needRebuildTransform = false;
}