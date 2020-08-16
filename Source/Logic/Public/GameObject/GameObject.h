#pragma once

#include "GameObjectProperty.h"
#include "Math/CXMFloat.h"
#include "Physics/Body.h"
#include "Physics/ColliderManager.h"
//#include "Render/Resource.h"
#include "Scene/INotifyGraphicsDevice.h"

#include <cstddef>
#include <limits>
#include <memory>

enum DIRTY_FLAG
{
	DF_POSITION = 1 << 0,
	DF_ROTATION = 1 << 1,
	DF_SCALING	= 1 << 2,
};

class CDebugOverlayManager;
class CGameLogic;
class CGameObject;
class ICollider;
class IMesh;

namespace JSON
{
	class Value;
}

class ObjectRelatedRigidBody : public RigidBody
{
public:
	CGameObject* GetGameObject( ) const { return m_gameObject; }
	const ICollider* GetCollider( int type );

	int GetDirty( ) const { return m_dirtyFlag; }
	void SetDirty( int dirtyFlag ) { m_dirtyFlag |= dirtyFlag; }
	void ResetDirty( ) { m_dirtyFlag = 0; }

	explicit ObjectRelatedRigidBody( CGameObject* object ) : m_gameObject( object ) {}

private:
	CGameObject* m_gameObject = nullptr;
	int m_dirtyFlag = 0;
};

class CGameObject : IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;
	virtual bool Initialize( CGameLogic& gameLogic );
	virtual void SetPosition( const float x, const float y, const float z );
	virtual void SetPosition( const CXMFLOAT3& pos );
	virtual void SetScale( const float xScale, const float yScale, const float zScale );
	virtual void SetRotate( const CXMFLOAT4& rotate );
	virtual void SetRotate( const float pitch, const float yaw, const float roll );
	virtual void SetRotate( const CXMFLOAT3& pitchYawRoll );

	std::size_t GetID( ) const { return m_id; }
	void SetID( std::size_t id ) { m_id = id; }

	const CXMFLOAT3& GetPosition( );
	const CXMFLOAT3& GetScale( );
	const CXMFLOAT4& GetRotate( );

	const CXMFLOAT4X4& GetTransformMatrix( );
	const CXMFLOAT4X4& GetInvTransformMatrix( );

	void UpdateTransform( CGameLogic& gameLogic );
	virtual void Render( CGameLogic& gameLogic );

	virtual void Think( float elapsedTime );
	virtual void PostThink( float elapsedTime );

	void SetName( const std::string& name ) { m_name = name; }
	const std::string& GetName( ) const { return m_name; }
	void SetMaterialName( const std::string& pMaterialName );
	void SetModelMeshName( const std::string& pModelName );
	const std::string& GetMeshName( ) const { return m_meshName; }

	COLLIDER::TYPE GetColliderType( ) const { return m_colliderType; }
	const ICollider* GetDefaultCollider( );
	const ICollider* GetCollider( int type );
	const std::vector<std::unique_ptr<ICollider>>& GetSubColliders( int type );

	void SetColliderType( COLLIDER::TYPE type ) { m_colliderType = type; }

	bool IsPicked( ) const { return m_isPicked; }
	void SetPicked( bool isPicked ) { m_isPicked = isPicked; }

	IMesh* GetModel( ) const { return m_pModel; }
	void SetModel( IMesh* pModel ) { m_pModel = pModel; }

	//Material GetMaterial( ) { return m_material; }
	//void SetMaterial( Material material ) { m_material = material; }

	//Material GetOverrideMaterial( ) { return m_overrideMaterial; }
	//void SetOverrideMaterial( Material material ) { m_overrideMaterial = material; }

	UINT GetProperty( ) const { return m_property; }
	void AddProperty( const GAMEOBJECT_PROPERTY property ) { m_property |= property; }
	void RemoveProperty( const GAMEOBJECT_PROPERTY property ) { m_property &= ~property; }

	bool WillRemove( ) const { return ( m_property & GAMEOBJECT_PROPERTY::REMOVE_ME ); }

	virtual void LoadProperty( const JSON::Value& json );

	virtual bool IgnorePicking( ) const { return false; }

	virtual bool ShouldDraw( ) const { return true; }
	virtual bool ShouldDrawShadow( ) const { return true; }

	ObjectRelatedRigidBody* GetRigidBody( ) { return &m_body; }

	CGameObject( );
	~CGameObject( ) = default ;

protected:
	virtual bool LoadModelMesh( CGameLogic& gameLogic );
	virtual bool LoadMaterial( CGameLogic& gameLogic );
	virtual void CalcOriginalCollider( );

private:
	void RebuildTransform( );
	void UpdateCollider( COLLIDER::TYPE type );
	void UpdateAllCollider( );
	void UpdateSubCollider( COLLIDER::TYPE type );

private:
	std::size_t m_id = std::numeric_limits<std::size_t>::max( );

	CXMFLOAT3 m_vecPos = { 0.f, 0.f, 0.f };
	CXMFLOAT3 m_vecScale = { 1.f, 1.f, 1.f };
	CXMFLOAT4 m_vecRotate = { 0.f, 0.f, 0.f, 1.f };

	CXMFLOAT4X4 m_matTransform;
	CXMFLOAT4X4 m_invMatTransform;

	IMesh* m_pModel = nullptr;
	//Material m_material = INVALID_MATERIAL;
	//Material m_overrideMaterial = INVALID_MATERIAL;

	std::string m_name;
	std::string m_materialName;
	std::string m_meshName;

	bool m_isPicked = false;

	COLLIDER::TYPE m_colliderType = COLLIDER::NONE;
	ICollider* m_originalColliders[COLLIDER::COUNT] = { nullptr, };
	std::unique_ptr<ICollider>	m_colliders[COLLIDER::COUNT];

	std::vector<std::unique_ptr<ICollider>> m_subColliders[COLLIDER::COUNT];

	UINT m_property = 0;

	ObjectRelatedRigidBody m_body{ this };

protected:
	bool m_needRebuildTransform = true;
};

inline void RemoveObject( CGameObject& object )
{
	object.AddProperty( GAMEOBJECT_PROPERTY::REMOVE_ME );
}