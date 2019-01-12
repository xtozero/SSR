#pragma once

#include "GameObjectProperty.h"
#include "Math/CXMFloat.h"
#include "Physics/Body.h"
#include "Physics/ColliderManager.h"
#include "Render/Resource.h"
#include "Scene/INotifyGraphicsDevice.h"

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
class KeyValue;

struct ObjectRelatedRigidBody : public RigidBody
{
	CGameObject* m_gameObject = nullptr;
};

class CGameObject : IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;
	virtual bool Initialize( CGameLogic& gameLogic, int id );
	virtual void SetPosition( const float x, const float y, const float z );
	virtual void SetPosition( const CXMFLOAT3& pos );
	virtual void SetScale( const float xScale, const float yScale, const float zScale );
	virtual void SetRotate( const float pitch, const float yaw, const float roll );
	virtual void SetRotate( const CXMFLOAT3& pitchYawRoll );

	int GetID( ) const { return m_id; }
	void SetID( int id ) { m_id = id; }

	const CXMFLOAT3& GetPosition( );
	const CXMFLOAT3& GetScale( );
	const CXMFLOAT3& GetRotate( );

	const CXMFLOAT4X4& GetTransformMatrix( );
	const CXMFLOAT4X4& GetInvTransformMatrix( );

	void UpdateTransform( CGameLogic& gameLogic );
	virtual void Render( CGameLogic& gameLogic );

	virtual void Think( );
	virtual void PostThink( );

	void SetName( const String& name ) { m_name = name; }
	const String& GetName( ) const { return m_name; }
	void SetMaterialName( const String& pMaterialName );
	void SetModelMeshName( const String& pModelName );
	const String& GetMeshName( ) const { return m_meshName; }

	COLLIDER::TYPE GetColliderType( ) const { return m_colliderType; }
	const ICollider* GetDefaultCollider( );
	const ICollider* GetCollider( int type );
	const std::vector<std::unique_ptr<ICollider>>& GetSubColliders( int type );
	void DrawDefaultCollider( CDebugOverlayManager& debugOverlay );

	bool IsPicked( ) const { return m_isPicked; }
	void SetPicked( bool isPicked ) { m_isPicked = isPicked; }

	IMesh* GetModel( ) const { return m_pModel; }
	void SetModel( IMesh* pModel ) { m_pModel = pModel; }

	Material GetMaterial( ) { return m_material; }
	void SetMaterial( Material material ) { m_material = material; }

	Material GetOverrideMaterial( ) { return m_overrideMaterial; }
	void SetOverrideMaterial( Material material ) { m_overrideMaterial = material; }

	UINT GetProperty( ) const { return m_property; }
	void AddProperty( const GAMEOBJECT_PROPERTY property ) { m_property |= property; }
	void RemoveProperty( const GAMEOBJECT_PROPERTY property ) { m_property &= ~property; }

	virtual void LoadPropertyFromScript( const KeyValue& keyValue );

	virtual bool IgnorePicking( ) const { return false; }

	virtual bool ShouldDraw( ) const { return true; }
	virtual bool ShouldDrawShadow( ) const { return true; }

	ObjectRelatedRigidBody* GetRigidBody( ) { return &m_body; }

	int GetDirty( ) const { return m_dirtyFlag; }
	void SetDirty( int dirtyFlag ) { m_dirtyFlag |= dirtyFlag; }

	CGameObject( );
	~CGameObject( ) = default ;

protected:
	virtual bool LoadModelMesh( CGameLogic& gameLogic );
	virtual bool LoadMaterial( CGameLogic& gameLogic );

private:
	void RebuildTransform( );
	void UpdateCollider( COLLIDER::TYPE type );
	void UpdateAllCollider( );
	void UpdateSubCollider( COLLIDER::TYPE type );

private:
	int m_id = -1;

	CXMFLOAT3 m_vecPos;
	CXMFLOAT3 m_vecScale;
	CXMFLOAT3 m_vecRotate;

	CXMFLOAT4X4 m_matTransform;
	CXMFLOAT4X4 m_invMatTransform;

	IMesh* m_pModel = nullptr;
	Material m_material = INVALID_MATERIAL;
	Material m_overrideMaterial = INVALID_MATERIAL;

	String m_name;
	String m_materialName;
	String m_meshName;

	bool m_isPicked = false;

	COLLIDER::TYPE m_colliderType = COLLIDER::NONE;
	ICollider* m_originalColliders[COLLIDER::COUNT] = { nullptr, };
	std::unique_ptr<ICollider>	m_colliders[COLLIDER::COUNT];

	std::vector<std::unique_ptr<ICollider>> m_subColliders[COLLIDER::COUNT];

	UINT m_property = 0;

	ObjectRelatedRigidBody m_body;

	int m_dirtyFlag = 0;

protected:
	bool m_needRebuildTransform = false;
};