#include "stdafx.h"

#include <tchar.h>

#include "GameObject.h"
#include "GameObjectFactory.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IMesh.h"
#include "Timer.h"

using namespace DirectX;

DECLARE_GAME_OBJECT( base, CGameObject );

void CGameObject::SetPosition( const float x, const float y, const float z )
{
	SetPosition( CXMFLOAT3( x, y, z ) );
}

void CGameObject::SetPosition( const CXMFLOAT3& pos )
{
	m_vecPos = pos;
	m_needRebuildTransform = true;
}

void CGameObject::SetScale( const float xScale, const float yScale, const float zScale )
{
	m_vecScale = CXMFLOAT3( xScale, yScale, zScale );
	m_needRebuildTransform = true;
}

void CGameObject::SetRotate( const float pitch, const float yaw, const float roll )
{
	m_vecRotate = CXMFLOAT3( pitch, yaw, roll );
	m_needRebuildTransform = true;
}

const CXMFLOAT3& CGameObject::GetPosition( )
{
	return m_vecPos;
}

const CXMFLOAT3& CGameObject::GetScale( )
{
	return m_vecScale;
}

const CXMFLOAT3& CGameObject::GetRotate( )
{
	return m_vecRotate;
}

const CXMFLOAT4X4& CGameObject::GetTransformMatrix( )
{
	if ( m_needRebuildTransform )
	{
		RebuildTransform( );
	}

	return m_matTransform;
}

const CXMFLOAT4X4& CGameObject::GetInvTransformMatrix( )
{
	if ( m_needRebuildTransform )
	{
		RebuildTransform( );
	}

	m_invMatTransform = XMMatrixInverse( nullptr, m_matTransform );

	return m_invMatTransform;
}

void CGameObject::UpdateWorldMatrix( IRenderer & renderer )
{
	renderer.UpdateWorldMatrix( GetTransformMatrix( ), GetInvTransformMatrix( ) );
}

void CGameObject::Render( IRenderer& renderer )
{
	if ( ShouldDraw() && m_pModel )
	{
		m_pModel->SetMaterial( m_pOverrideMtl ? m_pOverrideMtl : m_pMaterial );
		m_pModel->Draw( renderer );
	}
}

void CGameObject::Think( )
{
}

void CGameObject::SetMaterialName( const String& pMaterialName )
{
	m_materialName = pMaterialName;
}

void CGameObject::SetModelMeshName( const String& pModelName )
{
	m_meshName = pModelName;
}

bool CGameObject::Initialize( IRenderer& renderer )
{
	ON_FAIL_RETURN( LoadModelMesh( renderer ) );
	ON_FAIL_RETURN( LoadMaterial( renderer ) );

	m_needInitialize = false;
	return true;
}

bool CGameObject::LoadPropertyFromScript( const CKeyValueIterator& pKeyValue )
{
	if ( pKeyValue->GetKey( ) == String( _T( "Name" ) ) )
	{
		SetName( pKeyValue->GetValue( ) );
		return true;
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Model" ) ) )
	{
		SetModelMeshName( pKeyValue->GetValue( ).c_str( ) );
		return true;
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Position" ) ) )
	{
		std::vector<String> params;

		UTIL::Split( pKeyValue->GetValue( ), params, _T( ' ' ) );

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

		UTIL::Split( pKeyValue->GetValue( ), params, _T( ' ' ) );

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
		SetMaterialName( pKeyValue->GetValue( ) );
		return true;
	}
	else if ( pKeyValue->GetKey( ) == String( _T( "Reflectable" ) ) )
	{
		AddProperty( REFLECTABLE_OBJECT );
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
m_pOverrideMtl( nullptr ),
m_needInitialize( true ),
m_isPicked( false ),
m_needRebuildTransform( false ),
m_property( 0 )
{
	m_matTransform = XMMatrixIdentity();
	m_invMatTransform = XMMatrixIdentity( );

	for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
	{
		m_originRigidBodies[i] = nullptr;
		m_rigideBodies[i] = nullptr;
	}
}

bool CGameObject::LoadModelMesh( IRenderer& renderer )
{
	if ( m_pModel != nullptr )
	{
		return false;
	}

	if ( m_meshName.length( ) > 0 )
	{
		m_pModel = renderer.GetModelPtr( m_meshName.c_str( ) );

		LoadRigidBody( renderer );

		return m_pModel ? true : false;
	}

	return false;
}

bool CGameObject::LoadMaterial( IRenderer& renderer )
{
	if ( m_pModel )
	{
		if ( m_materialName.length( ) > 0 )
		{
			m_pMaterial = renderer.GetMaterialPtr( m_materialName.c_str( ) );
		}
		else
		{
			m_pMaterial = renderer.GetMaterialPtr( _T( "wireframe" ) );
		}
	}

	return m_pMaterial ? true : false;
}

void CGameObject::LoadRigidBody( IRenderer& renderer )
{
	for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
	{
		m_originRigidBodies[i] = CRigidBodyManager::GetInstance( ).GetRigidBody( renderer, m_meshName, static_cast<RIGID_BODY_TYPE>(i) );
	}
}

void CGameObject::RebuildTransform( )
{
	//STR
	XMMATRIX scale;
	XMMATRIX rotate;

	scale = XMMatrixScaling( m_vecScale.x, m_vecScale.y, m_vecScale.z );
	rotate = XMMatrixRotationRollPitchYaw( m_vecRotate.x, m_vecRotate.y, m_vecRotate.z );

	m_matTransform = scale * rotate;

	m_matTransform._41 = m_vecPos.x;
	m_matTransform._42 = m_vecPos.y;
	m_matTransform._43 = m_vecPos.z;

	UpdateRigidBodyAll( );
	m_needRebuildTransform = false;
}

void CGameObject::UpdateRigidBody( RIGID_BODY_TYPE type )
{
	if ( m_originRigidBodies[type] )
	{
		if ( m_rigideBodies[type].get( ) == nullptr )
		{
			m_rigideBodies[type].reset( CRigidBodyManager::GetInstance( ).MakeRigidBody( type ) );
		}

		m_rigideBodies[type]->Update( m_matTransform, m_originRigidBodies[type] );
	}
}

void CGameObject::UpdateRigidBodyAll( )
{
	for ( int i = 0; i < RIGID_BODY_TYPE::Count; ++i )
	{
		if ( m_originRigidBodies[i] )
		{
			UpdateRigidBody( static_cast<RIGID_BODY_TYPE>( i ) );
		}
	}
}