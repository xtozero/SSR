#pragma once

#include "common.h"
#include "CommonRenderer/Resource.h"

#include <memory>
#include <D3Dcommon.h>

class IMaterial;
class IRenderer;
class IRenderResource;
struct MeshVertex;
struct CXMFLOAT3;
struct ID3D11DeviceContext;
enum SHADER_TYPE;

class IMesh
{
public:
	virtual void SetModelData( MeshVertex* pOrignal, UINT vertexCount ) = 0;
	virtual void SetIndexData( void* pOrignal, UINT indexCount ) = 0;
	virtual void SetColor( const CXMFLOAT3& color ) = 0;
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) = 0;

	virtual void Draw( IRenderer& renderer ) = 0;

	virtual void SetMaterial( IMaterial* pMaterial ) = 0;
	virtual void SetTexture( IRenderResource* pTexture ) = 0;
	virtual IRenderResource* GetTexture( ) const = 0;

	virtual void* GetMeshData( ) = 0;
	virtual int GetVerticesCount( ) = 0;

	virtual ~IMesh( ) = default;
protected:
	IMesh( ) = default;
};

