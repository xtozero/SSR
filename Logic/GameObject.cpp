#include "stdafx.h"

#include <tchar.h>

#include "GameObject.h"
#include "GameObjectFactory.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/Direct3D11.h"
#include "../RenderCore/IMesh.h"
#include "Timer.h"

extern IRenderer* gRenderer;

DECLARE_GAME_OBJECT( base, CGameObject );

void CGameObject::SetPosition( const float x, const float y, const float z )
{
	SetPosition( D3DXVECTOR3( x, y, z ) );
}

void CGameObject::SetPosition( const D3DXVECTOR3& pos )
{
	m_vecPos = pos;
	m_needRebuildTransform = true;
}

void CGameObject::SetScale( const float xScale, const float yScale, const float zScale )
{
	m_vecScale = D3DXVECTOR3( xScale, yScale, zScale );
	m_needRebuildTransform = true;
}

void CGameObject::SetRotate( const float pitch, const float yaw, const float roll )
{
	m_vecRotate = D3DXVECTOR3( pitch, yaw, roll );
	m_needRebuildTransform = true;
}

const D3DXVECTOR3& CGameObject::GetPosition( )
{
	return m_vecPos;
}

const D3DXVECTOR3& CGameObject::GetScale( )
{
	return m_vecScale;
}

const D3DXVECTOR3& CGameObject::GetRotate( )
{
	return m_vecRotate;
}

const D3DXMATRIX& CGameObject::GetTransformMatrix( )
{
	if ( m_needRebuildTransform )
	{
		RebuildTransform( );
	}

	return m_matTransform;
}

void CGameObject::Render( )
{
	if ( ShouldDraw() && m_pModel )
	{
		m_pModel->SetMaterial( m_pMaterial );
		gRenderer->DrawModel( m_pModel );
	}
}

void CGameObject::Think( )
{
	//Test Code
	if ( m_isPicked )
	{

	}
	else
	{
		float fDeltaTime = CTimer::GetInstance( ).GetElapsedTIme( );
		const D3DXVECTOR3& curRotate = GetRotate( );

		SetRotate( curRotate.x, curRotate.y + D3DXToRadian( 45.f ) * fDeltaTime, 0.f );
	}
}

void CGameObject::SetMaterialName( const String& pMaterialName )
{
	m_materialName = pMaterialName;
}

void CGameObject::SetModelMeshName( const String& pModelName )
{
	m_meshName = pModelName;
}

bool CGameObject::Initialize( )
{
	ON_FAIL_RETURN( LoadModelMesh( ) );
	ON_FAIL_RETURN( LoadMaterial( ) );

	m_needInitialize = false;
	return true;
}

bool CGameObject::LoadPropertyFromScript( const CKeyValueIterator& pKeyValue )
{
	if ( pKeyValue->GetKey( ) == String( _T( "Name" ) ) )
	{
		SetName( pKeyValue->GetString( ) );
		return true;
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Model" ) ) )
	{
		SetModelMeshName( pKeyValue->GetString( ).c_str( ) );
		return true;
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Position" ) ) )
	{
		std::vector<String> params;

		UTIL::Split( pKeyValue->GetString( ), params, _T( ' ' ) );

		if ( params.size( ) == 3 )
		{
			float x = static_cast<float>(_ttof( params[0].c_str( ) ));
			float y = static_cast<float>(_ttof( params[1].c_str( ) ));
			float z = static_cast<float>(_ttof( params[2].c_str( ) ));

			SetPosition( x, y, z );
			return true;
		}
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Scale" ) ) )
	{
		std::vector<String> params;

		UTIL::Split( pKeyValue->GetString( ), params, _T( ' ' ) );

		if ( params.size( ) == 3 )
		{
			float x = static_cast<float>(_ttof( params[0].c_str( ) ));
			float y = static_cast<float>(_ttof( params[1].c_str( ) ));
			float z = static_cast<float>(_ttof( params[2].c_str( ) ));

			SetScale( x, y, z );
			return true;
		}
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Material" ) ) )
	{
		SetMaterialName( pKeyValue->GetString( ) );
		return true;
	}

	return false;
}

CGameObject::CGameObject( ) :
m_vecPos( 0.f, 0.f, 0.f ),
m_vecScale( 1.f, 1.f, 1.f ),
m_vecRotate( 0.f, 0.f, 0.f ),
m_pModel( nullptr ),
m_pMaterial( nullptr ),
m_needRebuildTransform( false ),
m_needInitialize( true ),
m_isPicked( false )
{
	D3DXMatrixIdentity( &m_matTransform );

	for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
	{
		m_originRigidBodies[i] = nullptr;
		m_rigideBodies[i] = nullptr;
	}
}

CGameObject::~CGameObject( )
{
}

bool CGameObject::LoadModelMesh( )
{
	if ( m_pModel != nullptr )
	{
		return false;
	}

	if ( m_meshName.length( ) > 0 )
	{
		m_pModel = gRenderer->GetModelPtr( m_meshName.c_str( ) );

		LoadRigidBody( );

		return m_pModel ? true : false;
	}

	return false;
}

bool CGameObject::LoadMaterial( )
{
	if ( m_pModel )
	{
		if ( m_materialName.length( ) > 0 )
		{
			m_pMaterial = gRenderer->GetMaterialPtr( m_materialName.c_str( ) );
		}
		else
		{
			m_pMaterial = gRenderer->GetMaterialPtr( _T( "wireframe" ) );
		}
	}

	return m_pMaterial ? true : false;
}

void CGameObject::LoadRigidBody( )
{
	for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
	{
		m_originRigidBodies[i] = CRigidBodyManager::GetInstance( ).GetRigidBody( m_meshName, static_cast<RIGID_BODY_TYPE>(i) );
	}
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

	UpdateRigidBodyAll( );
	m_needRebuildTransform = false;
}

void CGameObject::UpdateRigidBody( RIGID_BODY_TYPE type )
{
	if ( m_originRigidBodies[type].get( ) )
	{
		if ( m_rigideBodies[type].get( ) == nullptr )
		{
			m_rigideBodies[type] = CRigidBodyManager::GetInstance( ).MakeRigidBody( type );
		}

		m_rigideBodies[type]->Update( m_matTransform, m_originRigidBodies[type] );
	}
}

void CGameObject::UpdateRigidBodyAll( )
{
	for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
	{
		if ( m_originRigidBodies[i].get( ) )
		{
			UpdateRigidBody( static_cast<RIGID_BODY_TYPE>( i ) );
		}
	}
}