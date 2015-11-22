#pragma once

#include "common.h"
#include <d3d11.h>
#include "IBuffer.h"
#include "IMesh.h"

class IMaterial;

class RENDERCORE_DLL BaseMesh : public IMesh
{
protected:
	void* m_pModelData;
	void* m_pIndexData;

	IBuffer* m_pVertexBuffer;
	IBuffer* m_pIndexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;

	IMaterial* m_pMaterial;

	UINT m_nVertices;
	UINT m_nOffset;

	UINT m_nIndices;
	UINT m_nIndexOffset;
public:
	virtual void SetModelData( void* pOrignal, UINT vertexCount );
	virtual void SetIndexData( void* pOrignal, UINT indexCount );
	virtual bool Load( );

	virtual void Draw( ID3D11DeviceContext* pDeviceContext );

protected:
	bool LoadMaterial( const TCHAR* pMaterialName );

	BaseMesh( );

public:
	virtual ~BaseMesh( );
};

