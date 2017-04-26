#pragma once

#include "common.h"

#include <memory>
#include <D3Dcommon.h>

class IMaterial;
class IRenderer;
struct MeshVertex;
struct CXMFLOAT3;
struct ID3D11DeviceContext;
class IShaderResource;
enum SHADER_TYPE;

class IMesh
{
public:
	virtual void SetModelData( MeshVertex* pOrignal, UINT vertexCount ) = 0;
	virtual void SetIndexData( void* pOrignal, UINT indexCount ) = 0;
	virtual void SetColor( const CXMFLOAT3& color ) = 0;
	virtual bool Load( IRenderer& renderer, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) = 0;

	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) = 0;

	virtual void SetMaterial( IMaterial* pMaterial ) = 0;
	virtual void SetTexture( IShaderResource* pTexture ) = 0;
	virtual IShaderResource* GetTexture( ) const = 0;

	virtual void* GetMeshData( ) = 0;
	virtual int GetVerticesCount( ) = 0;

	virtual void ResetResource( ID3D11DeviceContext* pDeviceContext,  const SHADER_TYPE type ) = 0;

	virtual ~IMesh( ) = default;
protected:
	IMesh( ) = default;
};

