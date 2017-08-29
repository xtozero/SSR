#pragma once

#include "common.h"
#include "IBuffer.h"
#include "IMesh.h"
#include "../shared/Math/CXMFloat.h"

#include <memory>

class BaseMesh : public IMesh
{
protected:
	void* m_pModelData;
	void* m_pIndexData;

	std::shared_ptr<IBuffer> m_pVertexBuffer;
	std::shared_ptr<IBuffer> m_pIndexBuffer;

	D3D_PRIMITIVE_TOPOLOGY m_primitiveTopology;

	IMaterial* m_pMaterial;

	UINT m_nVertices;
	UINT m_nOffset;

	UINT m_nIndices;
	UINT m_nIndexOffset;

	CXMFLOAT3 m_color;
public:
	virtual void SetModelData( MeshVertex* pOrignal, UINT vertexCount ) override;
	virtual void SetIndexData( void* pOrignal, UINT indexCount ) override;
	virtual void SetColor( const CXMFLOAT3& color ) override;
	virtual bool Load( IRenderer& renderer, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) override;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	virtual void SetMaterial( IMaterial* pMaterial ) override;
	virtual void SetTexture( IShaderResource* ) override {}
	virtual IShaderResource* GetTexture( ) const override { return nullptr; }

	virtual void* GetMeshData( ) override { return m_pModelData; }
	virtual int GetVerticesCount( ) override { return m_nVertices; }

	virtual void ResetResource( ID3D11DeviceContext* pDeviceContext, const SHADER_TYPE type ) override;
protected:
	BaseMesh( );

public:
	virtual ~BaseMesh( );
};

