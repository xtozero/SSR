#pragma once

#include "common.h"
#include <d3d11.h>
#include "IBuffer.h"

class IMaterial;

class RENDERCORE_DLL BaseMesh
{
protected:
	void* m_pModelData;

	IBuffer* m_pVertexBuffer;
	IBuffer* m_pIndexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;

	IMaterial* m_pMaterial;

	UINT m_nVertices;
	UINT m_nOffset;
public:
	virtual void SetModelData( void* pOrignal, UINT vertexCount );
	virtual bool Load( );

	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) = 0;

protected:
	bool LoadMaterial( const TCHAR* pMaterialName );

	BaseMesh( );

public:
	virtual ~BaseMesh( );
};

