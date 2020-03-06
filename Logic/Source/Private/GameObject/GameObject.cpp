#include "stdafx.h"
#include "GameObject/GameObject.h"

#include "Core/GameLogic.h"
#include "Core/Timer.h"
#include "DataStructure/KeyValueReader.h"
#include "GameObject/GameObjectFactory.h"
#include "Math/Util.h"
#include "Model/IMesh.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/BoundingSphere.h"
#include "Physics/OrientedBoundingBox.h"
#include "Render/IRenderer.h"
#include "Scene/DebugOverlayManager.h"

#include <tchar.h>

using namespace DirectX;

const ICollider* ObjectRelatedRigidBody::GetCollider( int type )
{
	assert( m_gameObject != nullptr );
	return m_gameObject->GetCollider( type );
}

DECLARE_GAME_OBJECT( base, CGameObject );

void CGameObject::OnDeviceRestore( CGameLogic& gameLogic )
{
	m_material = INVALID_MATERIAL;
	m_overrideMaterial = INVALID_MATERIAL;

	m_pModel = nullptr;
	LoadModelMesh( gameLogic );
	LoadMaterial( gameLogic );
}

bool CGameObject::Initialize( CGameLogic& gameLogic, std::size_t id )
{
	if ( LoadModelMesh( gameLogic ) == false )
	{
		__debugbreak( );
	}

	if ( LoadMaterial( gameLogic ) == false )
	{
		__debugbreak( );
	}

	CalcOriginalCollider( );

	if ( const ICollider* defaultCollider = GetDefaultCollider( ) )
	{
		CXMFLOAT3X3 inertiaTensor;

		switch ( m_colliderType )
		{
		case COLLIDER::SPHERE:
		{
			const BoundingSphere* sphereCollider = static_cast<const BoundingSphere*>( defaultCollider );
			inertiaTensor = MakeSphereInertiaTensor( sphereCollider->GetRadius( ), m_body.GetMass( ) );
		}
		break;
		case COLLIDER::AABB:
		{
			const CAaboundingbox* boxCollider = static_cast<const CAaboundingbox*>( defaultCollider );
			CXMFLOAT3 halfSize;
			boxCollider->Size( halfSize );
			halfSize *= 0.5f;
			inertiaTensor = MakeBlockInertiaTensor( halfSize, m_body.GetMass( ) );
		}
		break;
		case COLLIDER::OBB:
		{
			const COrientedBoundingBox* boxCollider = static_cast<const COrientedBoundingBox*>( defaultCollider );
			inertiaTensor = MakeBlockInertiaTensor( boxCollider->GetHalfSize( ), m_body.GetMass( ) );
		}
		break;
		}

		m_body.SetInertiaTensor( inertiaTensor );
	}

	SetID( id );
	gameLogic.OnObjectSpawned( *this );

	return true;
}

void CGameObject::SetPosition( const float x, const float y, const float z )
{
	if ( m_vecPos.x == x && m_vecPos.y == y && m_vecPos.z == z )
	{
		return;
	}

	m_vecPos = CXMFLOAT3( x, y, z );
	m_needRebuildTransform = true;
	m_body.SetPosition( m_vecPos );
	m_body.SetDirty( DF_POSITION );
}

void CGameObject::SetPosition( const CXMFLOAT3& pos )
{
	SetPosition( pos.x, pos.y, pos.z );
}

void CGameObject::SetScale( const float xScale, const float yScale, const float zScale )
{
	if ( m_vecScale.x == xScale && m_vecScale.y == yScale && m_vecScale.z == zScale )
	{
		return;
	}

	m_vecScale = CXMFLOAT3( xScale, yScale, zScale );
	m_needRebuildTransform = true;
	m_body.SetDirty( DF_SCALING );
}

void CGameObject::SetRotate( const CXMFLOAT4& rotate )
{
	if ( m_vecRotate == rotate )
	{
		return;
	}

	m_vecRotate = rotate;
	m_needRebuildTransform = true;
	m_body.SetOrientation( m_vecRotate );
	m_body.SetDirty( DF_ROTATION );
}

void CGameObject::SetRotate( const float pitch, const float yaw, const float roll )
{
	SetRotate( static_cast<CXMFLOAT4>( XMQuaternionRotationRollPitchYaw( pitch, yaw, roll ) ) );
}

void CGameObject::SetRotate( const CXMFLOAT3& pitchYawRoll )
{
	SetRotate( static_cast<CXMFLOAT4>( XMQuaternionRotationRollPitchYaw( pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z ) ) );
}

const CXMFLOAT3& CGameObject::GetPosition( )
{
	return m_vecPos;
}

const CXMFLOAT3& CGameObject::GetScale( )
{
	return m_vecScale;
}

const CXMFLOAT4& CGameObject::GetRotate( )
{
	return m_vecRotate;
}

const CXMFLOAT4X4& CGameObject::GetTransformMatrix( )
{
	RebuildTransform( );
	return m_matTransform;
}

const CXMFLOAT4X4& CGameObject::GetInvTransformMatrix( )
{
	RebuildTransform( );
	return m_invMatTransform;
}

void CGameObject::UpdateTransform( CGameLogic& gameLogic )
{
	using namespace SHARED_CONSTANT_BUFFER;
	RE_HANDLE geometryBuffer = gameLogic.GetCommonConstantBuffer( VS_GEOMETRY );
	IRenderer& renderer = gameLogic.GetRenderer( );

	GeometryTransform* pDest = static_cast<GeometryTransform*>( renderer.LockBuffer( geometryBuffer ) );

	if ( pDest )
	{
		pDest->m_world = XMMatrixTranspose( GetTransformMatrix() );
		pDest->m_invWorld = XMMatrixTranspose( GetInvTransformMatrix() );

		renderer.UnLockBuffer( geometryBuffer );
		renderer.BindConstantBuffer( SHADER_TYPE::VS, VS_CONSTANT_BUFFER::GEOMETRY, 1, &geometryBuffer );
	}
	else
	{
		__debugbreak( );
	}
}

void CGameObject::Render( CGameLogic& gameLogic )
{
	if ( ShouldDraw() && m_pModel )
	{
		m_pModel->SetMaterial( ( m_overrideMaterial != INVALID_MATERIAL ) ? m_overrideMaterial : m_material );
		m_pModel->Draw( gameLogic );
	}
}

void CGameObject::Think( float /*elapsedTime*/ )
{
	if ( m_isPicked )
	{
		m_body.SetVelocity( CXMFLOAT3( 0.f, 0.f, 0.f ) );
		m_body.SetRotation( CXMFLOAT3( 0.f, 0.f, 0.f ) );
	}
}

void CGameObject::PostThink( float /*elapsedTime*/ )
{
	const CXMFLOAT3& newPos = m_body.GetPosition( );
	const CXMFLOAT4& newRot = m_body.GetOrientation( );

	bool needUpdatePos = m_vecPos != newPos;
	bool needUpdateRot = m_vecRotate != newRot;
	m_needRebuildTransform |= ( needUpdatePos || needUpdateRot );

	if ( needUpdatePos )
	{
		m_vecPos = newPos;
	}

	if ( needUpdateRot )
	{
		m_vecRotate = newRot;
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

const ICollider* CGameObject::GetDefaultCollider( )
{
	return GetCollider( m_colliderType );
}

const ICollider* CGameObject::GetCollider( int type )
{
	if ( ( m_colliderType == COLLIDER::NONE ) || ( type == COLLIDER::NONE ) )
	{
		return nullptr;
	}

	UpdateCollider( static_cast<COLLIDER::TYPE>( type ) );
	return m_colliders[type].get( );
}

const std::vector<std::unique_ptr<ICollider>>& CGameObject::GetSubColliders( int type )
{
	UpdateSubCollider( static_cast<COLLIDER::TYPE>( type ) );
	return m_subColliders[type];
}

void CGameObject::LoadPropertyFromScript( const KeyValue& keyValue )
{
	if ( const KeyValue* pName = keyValue.Find( _T( "Name" ) ) )
	{
		SetName( pName->GetValue( ) );
	}
	
	if ( const KeyValue* pModel = keyValue.Find( _T( "Model" ) ) )
	{
		SetModelMeshName( pModel->GetValue( ).c_str( ) );
	}
	
	if ( const KeyValue* pPos = keyValue.Find( _T( "Position" ) ) )
	{
		std::vector<String> params;

		UTIL::Split( pPos->GetValue( ), params, _T( ' ' ) );

		if ( params.size( ) == 3 )
		{
			float x = static_cast<float>(_ttof( params[0].c_str( ) ));
			float y = static_cast<float>(_ttof( params[1].c_str( ) ));
			float z = static_cast<float>(_ttof( params[2].c_str( ) ));

			SetPosition( x, y, z );
		}
	}
	
	if ( const KeyValue* pScale = keyValue.Find( _T( "Scale" ) ) )
	{
		std::vector<String> params;

		UTIL::Split( pScale->GetValue( ), params, _T( ' ' ) );

		if ( params.size( ) == 3 )
		{
			float x = static_cast<float>(_ttof( params[0].c_str( ) ));
			float y = static_cast<float>(_ttof( params[1].c_str( ) ));
			float z = static_cast<float>(_ttof( params[2].c_str( ) ));

			SetScale( x, y, z );
		}
	}

	if ( const KeyValue* pRotate = keyValue.Find( _T( "Rotate" ) ) )
	{
		std::vector<String> params;

		UTIL::Split( pRotate->GetValue( ), params, _T( ' ' ) );

		if ( params.size( ) == 3 )
		{
			float pitch = static_cast<float>( _ttof( params[0].c_str( ) ) );
			pitch = XMConvertToRadians( pitch );

			float yaw = static_cast<float>( _ttof( params[1].c_str( ) ) );
			yaw = XMConvertToRadians( yaw );

			float roll = static_cast<float>( _ttof( params[2].c_str( ) ) );
			roll = XMConvertToRadians( roll );

			SetRotate( pitch, yaw, roll );
		}
	}
	
	if ( const KeyValue* pMat = keyValue.Find( _T( "Material" ) ) )
	{
		SetMaterialName( pMat->GetValue( ) );
	}
	
	if ( const KeyValue* pReflectable = keyValue.Find( _T( "Reflectable" ) ) )
	{
		AddProperty( REFLECTABLE_OBJECT );
	}

	if ( const KeyValue* pColliderType = keyValue.Find( _T( "ColliderType" ) ) )
	{
		const String& type = pColliderType->GetValue();

		if ( type == _T( "Sphere" ) )
		{
			m_colliderType = COLLIDER::SPHERE;
		}
		else if ( type == _T( "AlignedBox" ) )
		{
			m_colliderType = COLLIDER::AABB;
		}
		else if ( type == _T( "Box" ) )
		{
			m_colliderType = COLLIDER::OBB;
		}
		else
		{
			__debugbreak( );
		}
	}

	if ( const KeyValue* pMass = keyValue.Find( _T( "Mass" ) ) )
	{
		assert( ( m_colliderType != COLLIDER::AABB ) || ( pMass->GetValue<float>( ) == FLT_MAX ) );
		m_body.SetMass( pMass->GetValue<float>( ) );
	}

	if ( const KeyValue* pDamping = keyValue.Find( _T( "Damping" ) ) )
	{
		std::vector<String> params;

		UTIL::Split( pDamping->GetValue( ), params, _T( ' ' ) );

		if ( params.size( ) == 2 )
		{
			m_body.SetLinearDamping( static_cast<float>( _ttof( params[0].c_str( ) ) ) );
			m_body.SetAngularDamping( static_cast<float>( _ttof( params[1].c_str( ) ) ) );
		}
	}
}

CGameObject::CGameObject( )
{
	m_matTransform = XMMatrixIdentity();
	m_invMatTransform = XMMatrixIdentity( );

	for ( int i = 0; i < COLLIDER::COUNT; ++i )
	{
		m_originalColliders[i] = nullptr;
		m_colliders[i] = nullptr;
	}
}

bool CGameObject::LoadModelMesh( CGameLogic& gameLogic )
{
	if ( m_pModel != nullptr )
	{
		return false;
	}

	if ( m_meshName.length( ) > 0 )
	{
		CModelManager& modelManager = gameLogic.GetModelManager( );
		m_pModel = modelManager.LoadMeshFromFile( gameLogic.GetRenderer(), m_meshName.c_str() );
	}

	return true;
}

bool CGameObject::LoadMaterial( CGameLogic& gameLogic )
{
	if ( m_pModel )
	{
		IRenderer& renderer = gameLogic.GetRenderer( );

		if ( m_materialName.length( ) > 0 )
		{
			m_material = renderer.SearchMaterial( m_materialName.c_str( ) );
		}
		else
		{
			m_material = renderer.SearchMaterial( _T( "wireframe" ) );
		}
	}

	return ( m_material != INVALID_MATERIAL ) ? true : false;
}

void CGameObject::CalcOriginalCollider( )
{
	if ( m_pModel == nullptr )
	{
		return;
	}

	for ( int i = 0; i < COLLIDER::COUNT; ++i )
	{
		m_originalColliders[i] = GetColliderManager( ).GetCollider( *m_pModel, static_cast<COLLIDER::TYPE>( i ) );
	}
}

void CGameObject::RebuildTransform( )
{
	if ( m_needRebuildTransform )
	{
		//STR
		XMMATRIX scale;
		XMMATRIX rotate;

		scale = XMMatrixScaling( m_vecScale.x, m_vecScale.y, m_vecScale.z );
		rotate = XMMatrixRotationQuaternion( m_vecRotate );

		m_matTransform = scale * rotate;

		m_matTransform._41 = m_vecPos.x;
		m_matTransform._42 = m_vecPos.y;
		m_matTransform._43 = m_vecPos.z;

		m_invMatTransform = XMMatrixInverse( nullptr, m_matTransform );

		m_needRebuildTransform = false;
	}
}

void CGameObject::UpdateCollider( COLLIDER::TYPE type )
{
	if ( m_originalColliders[type] )
	{
		if ( m_colliders[type].get( ) == nullptr )
		{
			m_colliders[type].reset( GetColliderManager( ).CreateCollider( type ) );
		}

		m_colliders[type]->Update( m_vecScale, m_body.GetOrientation(), m_body.GetPosition(), m_originalColliders[type] );
	}
}

void CGameObject::UpdateAllCollider( )
{
	for ( int i = 0; i < COLLIDER::COUNT; ++i )
	{
		if ( m_originalColliders[i] )
		{
			UpdateCollider( static_cast<COLLIDER::TYPE>( i ) );
		}
	}
}

void CGameObject::UpdateSubCollider( COLLIDER::TYPE type )
{
	UpdateCollider( type );

	if ( m_colliders[type].get( ) )
	{
		m_colliders[type]->CalcSubMeshBounds( m_subColliders[type] );
	}
}
