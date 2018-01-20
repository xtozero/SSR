#pragma once

#include "GameObjectProperty.h"
#include "INotifyGraphicsDevice.h"
#include "RigidBodyManager.h"

#include "../shared/Math/CXMFloat.h"

#include <memory>

class CGameLogic;
class IRigidBody;
class IMaterial;
class IMesh;
class KeyValue;

class CGameObject : IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;
	virtual void SetPosition( const float x, const float y, const float z );
	virtual void SetPosition( const CXMFLOAT3& pos );
	virtual void SetScale( const float xScale, const float yScale, const float zScale );
	virtual void SetRotate( const float pitch, const float yaw, const float roll );

	const CXMFLOAT3& GetPosition( );
	const CXMFLOAT3& GetScale( );
	const CXMFLOAT3& GetRotate( );

	const CXMFLOAT4X4& GetTransformMatrix( );
	const CXMFLOAT4X4& GetInvTransformMatrix( );

	void UpdateTransform( CGameLogic& gameLogic );
	virtual void Render( CGameLogic& gameLogic );

	virtual void Think( );

	void SetName( const String& name ) { m_name = name; }
	const String& GetName( ) const { return m_name; }
	void SetMaterialName( const String& pMaterialName );
	void SetModelMeshName( const String& pModelName );
	const String& GetMeshName( ) const { return m_meshName; }

	bool Initialize( CGameLogic& gameLogic );
	bool NeedInitialize( ) { return m_needInitialize; }
	const IRigidBody* GetRigidBody( int type ) const { return m_rigideBodies[type].get( ); }

	void SetPicked( bool isPicked ) { m_isPicked = isPicked; }

	IMesh* GetModel( ) const { return m_pModel; }
	void SetModel( IMesh* pModel ) { m_pModel = pModel; }

	IMaterial* GetMaterial( ) { return m_pMaterial; }
	void SetMaterial( IMaterial* pMaterial ) { m_pMaterial = pMaterial; }

	IMaterial* GetOverrideMaterial( ) { return m_pOverrideMtl; }
	void SetOverrideMaterial( IMaterial* pMaterial ) { m_pOverrideMtl = pMaterial; }

	UINT GetProperty( ) const { return m_property; }
	void AddProperty( const GAMEOBJECT_PROPERTY property ) { m_property |= property; }
	void RemoveProperty( const GAMEOBJECT_PROPERTY property ) { m_property &= ~property; }

	virtual void LoadPropertyFromScript( const KeyValue& keyValue );

	virtual bool IgnorePicking( ) const { return false; }

	virtual bool ShouldDraw( ) const { return true; }
	virtual bool ShouldDrawShadow( ) const { return true; }

	CGameObject( );
	~CGameObject( ) = default ;
protected:
	virtual bool LoadModelMesh( CGameLogic& gameLogic );
	virtual bool LoadMaterial( CGameLogic& gameLogic );
private:
	void RebuildTransform( );
	void UpdateRigidBody( RIGID_BODY_TYPE type );
	void UpdateRigidBodyAll( );

private:
	CXMFLOAT3 m_vecPos;
	CXMFLOAT3 m_vecScale;
	CXMFLOAT3 m_vecRotate;

	CXMFLOAT4X4 m_matTransform;
	CXMFLOAT4X4 m_invMatTransform;

	IMesh* m_pModel;
	IMaterial* m_pMaterial;
	IMaterial* m_pOverrideMtl;

	String m_name;
	String m_materialName;
	String m_meshName;

	bool m_needInitialize;
	bool m_isPicked;

	IRigidBody* m_originRigidBodies[RIGID_BODY_TYPE::Count] = { nullptr, };
	std::unique_ptr<IRigidBody>	m_rigideBodies[RIGID_BODY_TYPE::Count];

	UINT m_property;
protected:
	bool m_needRebuildTransform;
};