#pragma once

#include "common.h"
#include <d3d11.h>
#include "IBuffer.h"
#include "IMesh.h"
#include <memory>

class IMaterial;
struct MeshVertex;

class BaseMesh : public IMesh
{
protected:
	void* m_pModelData;
	void* m_pIndexData;

	std::shared_ptr<IBuffer> m_pVertexBuffer;
	std::shared_ptr<IBuffer> m_pIndexBuffer;

	D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;

	std::shared_ptr<IMaterial> m_pMaterial;

	UINT m_nVertices;
	UINT m_nOffset;

	UINT m_nIndices;
	UINT m_nIndexOffset;

	D3DXVECTOR3 m_color;
public:
	virtual void SetModelData( MeshVertex* pOrignal, UINT vertexCount ) override;
	virtual void SetIndexData( void* pOrignal, UINT indexCount ) override;
	virtual void SetColor( const D3DXVECTOR3& color ) override;
	virtual bool Load( ) override;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	virtual void SetMaterial( const std::shared_ptr<IMaterial> pMaterial ) override;

	virtual void* GetMeshData( ) override { return m_pModelData; }
	virtual int GetVerticesCount( ) override { return m_nVertices; }

protected:
	BaseMesh( );

public:
	virtual ~BaseMesh( );
};

