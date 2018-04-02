#pragma once

#include "common.h"
#include "IMesh.h"
#include "Math/CXMFloat.h"
#include "Util.h"

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

	virtual void Draw( CGameLogic& ) override {};

	virtual void SetMaterial( Material material ) override;
	virtual void SetTexture( RE_HANDLE ) override {}
	virtual RE_HANDLE GetTexture( ) const override { return RE_HANDLE_TYPE::INVALID_HANDLE; }

	virtual const void* GetMeshData( ) const override { return m_pModelData; }
	virtual int GetVerticesCount( ) const override { return m_nVertices; }

	virtual ~BaseMesh( );

protected:
	void* m_pModelData = nullptr;
	void* m_pIndexData = nullptr;

	RE_HANDLE m_vertexBuffer = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_indexBuffer = RE_HANDLE_TYPE::INVALID_HANDLE;

	UINT m_primitiveTopology = RESOURCE_PRIMITIVE::UNDEFINED;

	Material m_material = INVALID_MATERIAL;

	UINT m_nVertices = 0;
	UINT m_stride = 0;
	UINT m_offset = 0;

	UINT m_nIndices = 0;
	UINT m_nIndexOffset = 0;

	CXMFLOAT3 m_color;

	TCHAR m_name[MAX_MESH_NAME + 1] = {};
};

