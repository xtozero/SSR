#pragma once

#include <memory>

#include "GameObjectProperty.h"
#include "RigidBodyManager.h"

class IRenderer;
class IRigidBody;
class IMaterial;
class IMesh;
class CKeyValueIterator;

class CGameObject
{
public:
	virtual void SetPosition( const float x, const float y, const float z );
	virtual void SetPosition( const D3DXVECTOR3& pos );
	virtual void SetScale( const float xScale, const float yScale, const float zScale );
	virtual void SetRotate( const float pitch, const float yaw, const float roll );

	const D3DXVECTOR3& GetPosition( );
	const D3DXVECTOR3& GetScale( );
	const D3DXVECTOR3& GetRotate( );

	const D3DXMATRIX& GetTransformMatrix( );
	const D3DXMATRIX& GetInvTransformMatrix( );

	virtual void Render( IRenderer& renderer );

	virtual void Think( );

	void SetName( const String& name ) { m_name = name; }
	const String& GetName( ) const { return m_name; }
	void SetMaterialName( const String& pMaterialName );
	void SetModelMeshName( const String& pModelName );
	const String& GetMeshName( ) const { return m_meshName; }

	bool Initialize( IRenderer& renderer );
	bool NeedInitialize( ) { return m_needInitialize; }
	const IRigidBody* GetRigidBody( int type ) const { return m_rigideBodies[type].get( ); }

	void SetPicked( bool isPicked ) { m_isPicked = isPicked; }

	std::shared_ptr<IMesh> GetModel( ) const { return m_pModel; }
	void SetModel( const std::shared_ptr<IMesh>& pModel ) { m_pModel = pModel; }

	IMaterial* GetMaterial( ) { return m_pMaterial; }
	void SetMaterial( IMaterial* pMaterial ) { m_pMaterial = pMaterial; }

	IMaterial* GetOverrideMaterial( ) { return m_pOverrideMtl; }
	void SetOverrideMaterial( IMaterial* pMaterial ) { m_pOverrideMtl = pMaterial; }

	UINT GetProperty( ) const { return m_property; }
	void AddProperty( const GAMEOBJECT_PROPERTY property ) { m_property |= property; }
	void RemoveProperty( const GAMEOBJECT_PROPERTY property ) { m_property &= ~property; }

	virtual bool LoadPropertyFromScript( const CKeyValueIterator& pKeyValue );

	virtual bool IgnorePicking( ) const { return false; }

	virtual bool ShouldDraw( ) const { return true; }
	virtual bool ShouldDrawShadow( ) const { return true; }

	CGameObject( );
	~CGameObject( ) = default ;
protected:
	virtual bool LoadModelMesh( IRenderer& renderer );
	virtual bool LoadMaterial( IRenderer& renderer );
	void LoadRigidBody( IRenderer& renderer );
private:
	void RebuildTransform( );
	void UpdateRigidBody( RIGID_BODY_TYPE type );
	void UpdateRigidBodyAll( );

private:
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecScale;
	D3DXVECTOR3 m_vecRotate;

	D3DXMATRIX m_matTransform;
	D3DXMATRIX m_invMatTransform;

	std::shared_ptr<IMesh> m_pModel;
	IMaterial* m_pMaterial;
	IMaterial* m_pOverrideMtl;

	String m_name;
	String m_materialName;
	String m_meshName;

	bool m_needInitialize;
	bool m_isPicked;

	std::shared_ptr<IRigidBody> m_originRigidBodies[RIGID_BODY_TYPE::Count];
	std::unique_ptr<IRigidBody>	m_rigideBodies[RIGID_BODY_TYPE::Count];

	UINT m_property;
protected:
	bool m_needRebuildTransform;
};