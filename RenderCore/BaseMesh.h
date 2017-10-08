#pragma once

#include "common.h"
#include "CommonRenderer/IBuffer.h"
#include "IMesh.h"
#include "../shared/Math/CXMFloat.h"
#include "../shared/Util.h"

#include <memory>

class BaseMesh : public IMesh
{
protected:
	void* m_pModelData;
	void* m_pIndexData;

	IBuffer* m_pVertexBuffer;
	IBuffer* m_pIndexBuffer;

	UINT m_primitiveTopology;

	IMaterial* m_pMaterial;

	UINT m_nVertices;
	UINT m_stride;
	UINT m_nOffset;

	UINT m_nIndices;
	UINT m_nIndexOffset;

	CXMFLOAT3 m_color;
public:
	virtual void SetModelData( Owner<MeshVertex*> pOrignal, UINT vertexCount ) override;
	virtual void SetIndexData( Owner<void*> pOrignal, UINT indexCount ) override;
	virtual void SetColor( const CXMFLOAT3& color ) override;
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;

	virtual void Draw( IRenderer& renderer ) override;

	virtual void SetMaterial( IMaterial* pMaterial ) override;
	virtual void SetTexture( IRenderResource* ) override {}
	virtual IRenderResource* GetTexture( ) const override { return nullptr; }

	virtual void* GetMeshData( ) override { return m_pModelData; }
	virtual int GetVerticesCount( ) override { return m_nVertices; }

protected:
	BaseMesh( );

public:
	virtual ~BaseMesh( );
};

