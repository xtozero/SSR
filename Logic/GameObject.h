#pragma once

#include "common.h"
#include <d3dx9math.h>
#include "../RenderCore/IMesh.h"
#include "RigidBodyManager.h"
#include <memory>

class IRigidBody;

class CGameObject
{
public:
	void SetPosition( const float x, const float y, const float z );
	void SetPosition( const D3DXVECTOR3& pos );
	void SetScale( const float xScale, const float yScale, const float zScale );
	void SetRotate( const float pitch, const float yaw, const float roll );

	const D3DXVECTOR3& GetPosition( );
	const D3DXVECTOR3& GetScale( );
	const D3DXVECTOR3& GetRotate( );
	
	const D3DXMATRIX& GetTransformMatrix( ) const;

	void Render( );
	
	void Think( );

	void SetName( const String& name ) { m_name = name; }
	const String& GetName( ) const { return m_name; }
	void SetMaterialName( const TCHAR* pMaterialName );
	void SetModelMeshName( const TCHAR* pModelName );
	const String& GetMeshName( ) const { return m_meshName; }
	
	bool Initialize( );
	bool NeedInitialize( ) { return m_needInitialize; }
	const IRigidBody* GetRigidBody( int type ) const { return m_rigideBodies[type].get(); }

	void SetPicked( bool isPicked ) { m_isPicked = isPicked; }

	CGameObject( );
	~CGameObject( );
protected:
	bool LoadModelMesh( );
	bool LoadMaterial( );
private:
	virtual bool ShouldDraw( ) { return true; }
	void RebuildTransform( );
	void UpdateRigidBody( RIGID_BODY_TYPE type );
	void UpdateRigidBodyAll( );

private:
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecScale;
	D3DXVECTOR3 m_vecRotate;

	D3DXMATRIX m_matTransform;

	std::shared_ptr<IMesh> m_pModel;
	std::shared_ptr<IMaterial> m_pMaterial;

	String m_name;
	String m_materialName;
	String m_meshName;

	bool m_needRebuildTransform;
	bool m_needInitialize;
	bool m_isPicked;

	std::shared_ptr<IRigidBody> m_originRigidBodies[RIGID_BODY_TYPE::Count];
	std::unique_ptr<IRigidBody>	m_rigideBodies[RIGID_BODY_TYPE::Count];
};

