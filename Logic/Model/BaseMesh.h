#pragma once

#include "IMesh.h"

#include "../common.h"
#include "../../RenderCore/CommonRenderer/IBuffer.h"
#include "../../shared/Math/CXMFloat.h"
#include "../../shared/Util.h"

#include <memory>

class BaseMesh : public IMesh
{
public:
	virtual const TCHAR* GetName( ) const override { return m_name; }
	virtual void SetName( const TCHAR* name ) override;

	virtual void SetModelData( Owner<MeshVertex*> pOrignal, UINT vertexCount ) override;
	virtual void SetIndexData( Owner<void*> pOrignal, UINT indexCount ) override;
	virtual void SetColor( const CXMFLOAT3& color ) override;
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;

	virtual void Draw( CGameLogic& renderer ) override;

	virtual void SetMaterial( IMaterial* pMaterial ) override;
	virtual void SetTexture( IRenderResource* ) override {}
	virtual IRenderResource* GetTexture( ) const override { return nullptr; }

	virtual const void* GetMeshData( ) const override { return m_pModelData; }
	virtual int GetVerticesCount( ) const override { return m_nVertices; }

	virtual ~BaseMesh( );

protected:
	void* m_pModelData = nullptr;
	void* m_pIndexData = nullptr;

	IBuffer* m_pVertexBuffer = nullptr;
	IBuffer* m_pIndexBuffer = nullptr;

	UINT m_primitiveTopology = RESOURCE_PRIMITIVE::UNDEFINED;

	IMaterial* m_pMaterial = nullptr;

	UINT m_nVertices = 0;
	UINT m_stride = 0;
	UINT m_nOffset = 0;

	UINT m_nIndices = 0;
	UINT m_nIndexOffset = 0;

	CXMFLOAT3 m_color;

	TCHAR m_name[MAX_MESH_NAME + 1] = {};
};

