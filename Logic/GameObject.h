#pragma once

#include "common.h"
#include <d3dx9math.h>
#include "..\RenderCore\IMesh.h"
#include <memory>

class CGameObject
{
public:
	void SetPosition( const float x, const float y, const float z );
	void SetScale( const float xScale, const float yScale, const float zScale );
	inline void SetRotate( const float pitch, const float yaw, const float roll );

	inline const D3DXVECTOR3& GetPosition( );
	inline const D3DXVECTOR3& GetScale( );
	inline const D3DXVECTOR3& GetRotate( );
	
	const D3DXMATRIX& GetTransformMatrix( ) const;

	void LoadModelMesh( const TCHAR* modelName );
	void Render( );
	
	void Think( );

	CGameObject( );
	~CGameObject( );
private:
	virtual bool ShouldDraw( ) { return true; }
	void RebuildTransform( );

private:
	D3DXVECTOR3 m_vecPos;
	D3DXVECTOR3 m_vecScale;
	D3DXVECTOR3 m_vecRotate;

	D3DXMATRIX m_matTransform;

	std::shared_ptr<IMesh> m_pModel;

	String m_meshName;

	bool m_needRebuildTransform;
};

