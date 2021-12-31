#include "stdafx.h"
#include "GameObject/GameObject.h"

#include "Components/Component.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Core/GameLogic.h"
#include "Core/Timer.h"
#include "GameObject/GameObjectFactory.h"
#include "InterfaceFactories.h"
#include "Json/json.hpp"
#include "Math/Util.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/BoundingSphere.h"
#include "Physics/OrientedBoundingBox.h"
//#include "Render/IRenderer.h"
#include "Scene/DebugOverlayManager.h"

#include <tchar.h>

using namespace DirectX;

const ICollider* ObjectRelatedRigidBody::GetCollider( int32 type )
{
	assert( m_gameObject != nullptr );
	return m_gameObject->GetCollider( type );
}

void CGameObject::OnDeviceRestore( CGameLogic& gameLogic )
{
	//m_material = INVALID_MATERIAL;
	//m_overrideMaterial = INVALID_MATERIAL;

	//LoadModelMesh( gameLogic );
	//LoadMaterial( gameLogic );
}

void CGameObject::Initialize( CGameLogic& gameLogic, World& world )
{
	m_pWorld = &world;

	for ( Component* component : m_components )
	{
		component->RegisterComponent( );
	}

	//if ( LoadMaterial( gameLogic ) == false )
	//{
	//	__debugbreak( );
	//}

	//CalcOriginalCollider( );

	//if ( const ICollider* defaultCollider = GetDefaultCollider( ) )
	//{
	//	CXMFLOAT3X3 inertiaTensor;

	//	switch ( m_colliderType )
	//	{
	//	case COLLIDER::SPHERE:
	//	{
	//		const BoundingSphere* sphereCollider = static_cast<const BoundingSphere*>( defaultCollider );
	//		inertiaTensor = MakeSphereInertiaTensor( sphereCollider->GetRadius( ), m_body.GetMass( ) );
	//	}
	//	break;
	//	case COLLIDER::AABB:
	//	{
	//		const CAaboundingbox* boxCollider = static_cast<const CAaboundingbox*>( defaultCollider );
	//		CXMFLOAT3 halfSize;
	//		boxCollider->Size( halfSize );
	//		halfSize *= 0.5f;
	//		inertiaTensor = MakeBlockInertiaTensor( halfSize, m_body.GetMass( ) );
	//	}
	//	break;
	//	case COLLIDER::OBB:
	//	{
	//		const COrientedBoundingBox* boxCollider = static_cast<const COrientedBoundingBox*>( defaultCollider );
	//		inertiaTensor = MakeBlockInertiaTensor( boxCollider->GetHalfSize( ), m_body.GetMass( ) );
	//	}
	//	break;
	//	}

	//	m_body.SetInertiaTensor( inertiaTensor );
	//}

	// SetID( id );
}

void CGameObject::SetPosition( const float x, const float y, const float z )
{
	if ( m_rootComponent )
	{
		m_rootComponent->SetPosition( x, y, z );
	}
}

void CGameObject::SetPosition( const Vector& pos )
{
	SetPosition( pos.x, pos.y, pos.z );
}

void CGameObject::SetScale( const float xScale, const float yScale, const float zScale )
{
	if ( m_rootComponent )
	{
		m_rootComponent->SetScale( xScale, yScale, zScale );
	}

	// m_body.SetDirty( DF_SCALING );
}

void CGameObject::SetRotate( const Quaternion& rotate )
{
	if ( m_rootComponent )
	{
		m_rootComponent->SetRotate( rotate );
	}

	// m_body.SetOrientation( m_vecRotate );
	// m_body.SetDirty( DF_ROTATION );
}

const Vector& CGameObject::GetPosition( )
{
	if ( m_rootComponent )
	{
		return m_rootComponent->GetPosition( );
	}

	return Vector::ZeroVector;
}

const Vector& CGameObject::GetScale( )
{
	if ( m_rootComponent )
	{
		return m_rootComponent->GetScale( );
	}

	return Vector::OneVector;
}

const Quaternion& CGameObject::GetRotate( )
{
	if ( m_rootComponent )
	{
		return m_rootComponent->GetRotate( );
	}

	return Quaternion::Identity;
}

const Matrix& CGameObject::GetTransformMatrix( )
{
	if ( m_rootComponent )
	{
		return m_rootComponent->GetTransformMatrix( );
	}

	return Matrix::Identity;
}

const Matrix& CGameObject::GetInvTransformMatrix( )
{
	if ( m_rootComponent )
	{
		return m_rootComponent->GetInvTransformMatrix( );
	}

	return Matrix::Identity;
}

void CGameObject::UpdateTransform( CGameLogic& gameLogic )
{
	//using namespace SHARED_CONSTANT_BUFFER;
	//RE_HANDLE geometryBuffer = gameLogic.GetCommonConstantBuffer( VS_GEOMETRY );
	//IRenderer& renderer = gameLogic.GetRenderer( );

	//GeometryTransform* pDest = static_cast<GeometryTransform*>( renderer.LockBuffer( geometryBuffer ) );

	//if ( pDest )
	//{
	//	pDest->m_world = XMMatrixTranspose( GetTransformMatrix() );
	//	pDest->m_invWorld = XMMatrixTranspose( GetInvTransformMatrix() );

	//	renderer.UnLockBuffer( geometryBuffer );
	//	renderer.BindConstantBuffer( SHADER_TYPE::VS, VS_CONSTANT_BUFFER::GEOMETRY, 1, &geometryBuffer );
	//}
	//else
	//{
	//	__debugbreak( );
	//}
}

void CGameObject::Think( float elapsedTime )
{
	if ( m_isPicked )
	{
		//m_body.SetVelocity( CXMFLOAT3( 0.f, 0.f, 0.f ) );
		//m_body.SetRotation( CXMFLOAT3( 0.f, 0.f, 0.f ) );
	}

	for ( Component* component : m_components )
	{
		component->ThinkComponent( elapsedTime );
	}
}

void CGameObject::PostThink( float /*elapsedTime*/ )
{
	//const CXMFLOAT3& newPos = m_body.GetPosition( );
	//const CXMFLOAT4& newRot = m_body.GetOrientation( );

	//bool needUpdatePos = m_vecPos != newPos;
	//bool needUpdateRot = m_vecRotate != newRot;

	//if ( needUpdatePos )
	//{
	//	m_vecPos = newPos;
	//}

	//if ( needUpdateRot )
	//{
	//	m_vecRotate = newRot;
	//}
}

void CGameObject::SetMaterialName( const std::string& pMaterialName )
{
	m_materialName = pMaterialName;
}

const ICollider* CGameObject::GetDefaultCollider( )
{
	return GetCollider( m_colliderType );
}

const ICollider* CGameObject::GetCollider( int32 type )
{
	if ( ( m_colliderType == COLLIDER::NONE ) || ( type == COLLIDER::NONE ) )
	{
		return nullptr;
	}

	UpdateCollider( static_cast<COLLIDER::TYPE>( type ) );
	return m_colliders[type].get( );
}

const std::vector<std::unique_ptr<ICollider>>& CGameObject::GetSubColliders( int32 type )
{
	UpdateSubCollider( static_cast<COLLIDER::TYPE>( type ) );
	return m_subColliders[type];
}

void CGameObject::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	if ( const JSON::Value* pName = json.Find( "Name" ) )
	{
		SetName( pName->AsString( ) );
	}
	
	if ( const JSON::Value* pPos = json.Find( "Position" ) )
	{
		const JSON::Value& pos = *pPos;

		if ( pos.Size( ) == 3 )
		{
			float x = static_cast<float>( pos[0].AsReal( ) );
			float y = static_cast<float>( pos[1].AsReal( ) );
			float z = static_cast<float>( pos[2].AsReal( ) );

			SetPosition( x, y, z );
		}
	}
	
	if ( const JSON::Value* pScale = json.Find( "Scale" ) )
	{
		const JSON::Value& scale = *pScale;

		if ( scale.Size( ) == 3 )
		{
			float x = static_cast<float>( scale[0].AsReal( ) );
			float y = static_cast<float>( scale[1].AsReal( ) );
			float z = static_cast<float>( scale[2].AsReal( ) );

			SetScale( x, y, z );
		}
	}

	if ( const JSON::Value* pRotate = json.Find( "Rotate" ) )
	{
		const JSON::Value& rotate = *pRotate;

		if ( rotate.Size( ) == 3 )
		{
			float pitch = static_cast<float>( rotate[0].AsReal( ) );
			pitch = XMConvertToRadians( pitch );

			float yaw = static_cast<float>( rotate[1].AsReal( ) );
			yaw = XMConvertToRadians( yaw );

			float roll = static_cast<float>( rotate[2].AsReal( ) );
			roll = XMConvertToRadians( roll );

			SetRotate( Quaternion( pitch, yaw, roll ) );
		}
	}
	
	//if ( const JSON::Value* pRenderOption = json.Find( "RenderOption" ) )
	//{
	//	// SetMaterialName( pMat->AsString( ) );
	//	m_pRenderOption = GetInterface<IAssetLoader>( )->GetRenderOption( pRenderOption->AsString() );
	//}
	
	if ( const JSON::Value* pReflectable = json.Find( "Reflectable" ) )
	{
		AddProperty( REFLECTABLE_OBJECT );
	}

	if ( const JSON::Value* pColliderType = json.Find(  "ColliderType" ) )
	{
		const std::string& type = pColliderType->AsString();

		if ( type == "Sphere" )
		{
			m_colliderType = COLLIDER::SPHERE;
		}
		else if ( type == "AlignedBox" )
		{
			m_colliderType = COLLIDER::AABB;
		}
		else if ( type == "Box" )
		{
			m_colliderType = COLLIDER::OBB;
		}
		else
		{
			__debugbreak( );
		}
	}

	if ( const JSON::Value* pMass = json.Find( "Mass" ) )
	{
		float mass = static_cast<float>( pMass->AsReal( ) );
		assert( ( m_colliderType != COLLIDER::AABB ) || ( mass == FLT_MAX ) );
		if ( PrimitiveComponent* primitiveComponent = dynamic_cast<PrimitiveComponent*>( m_rootComponent ) )
		{
			primitiveComponent->SetMass( mass );
		}
	}

	if ( const JSON::Value* pDamping = json.Find( "Damping" ) )
	{
		const JSON::Value& damping = *pDamping;

		if ( damping.Size( ) == 2 )
		{
			if ( PrimitiveComponent* primitiveComponent = dynamic_cast<PrimitiveComponent*>( m_rootComponent ) )
			{
				primitiveComponent->SetLinearDamping( static_cast<float>( damping[0].AsReal( ) ) );
				primitiveComponent->SetAngularDamping( static_cast<float>( damping[1].AsReal( ) ) );
			}
		}
	}
}

void CGameObject::SetInputController( InputController* inputController )
{
	m_inputController = inputController;
}

InputComponent* CGameObject::GetInputComponent( )
{
	return m_inputComponent;
}

void CGameObject::InitializeInputComponent( )
{
	if ( m_inputComponent == nullptr )
	{
		m_inputComponent = CreateComponent<InputComponent>( *this );
		SetupInputComponent( );
	}
}

CGameObject::CGameObject( )
{
	for ( uint32 i = 0; i < COLLIDER::COUNT; ++i )
	{
		m_originalColliders[i] = nullptr;
		m_colliders[i] = nullptr;
	}
}

CGameObject::~CGameObject( )
{
	if ( m_inputController )
	{
		m_inputController->Abandon( );
	}

	for ( Component* component : m_components )
	{
		component->UnregisterComponent( );
		delete component;
	}
}

//bool CGameObject::LoadMaterial( CGameLogic& gameLogic )
//{
	//if ( m_pModel )
	//{
	//	IRenderer& renderer = gameLogic.GetRenderer( );

	//	if ( m_materialName.length( ) > 0 )
	//	{
	//		m_material = renderer.SearchMaterial( m_materialName.c_str( ) );
	//	}
	//	else
	//	{
	//		m_material = renderer.SearchMaterial( "wireframe" );
	//	}
	//}

	//return ( m_material != INVALID_MATERIAL ) ? true : false;

//	return true;
//}

void CGameObject::CalcOriginalCollider( )
{
	//if ( m_pModel == nullptr )
	//{
	//	return;
	//}

	//for ( uint32 i = 0; i < COLLIDER::COUNT; ++i )
	//{
	//	m_originalColliders[i] = GetColliderManager( ).GetCollider( *m_pModel, static_cast<COLLIDER::TYPE>( i ) );
	//}
}

void CGameObject::SetupInputComponent( )
{
}

void CGameObject::UpdateCollider( COLLIDER::TYPE type )
{
	//if ( m_originalColliders[type] )
	//{
	//	if ( m_colliders[type].get( ) == nullptr )
	//	{
	//		m_colliders[type].reset( GetColliderManager( ).CreateCollider( type ) );
	//	}

	//	m_colliders[type]->Update( m_vecScale, m_body.GetOrientation(), m_body.GetPosition(), m_originalColliders[type] );
	//}
}

void CGameObject::UpdateAllCollider( )
{
	for ( uint32 i = 0; i < COLLIDER::COUNT; ++i )
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

void RemoveObject( CGameObject& object )
{
	object.AddProperty( GAMEOBJECT_PROPERTY::REMOVE_ME );
}
