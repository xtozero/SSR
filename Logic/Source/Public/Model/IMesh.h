#pragma once

#include "common.h"
#include "Render/Resource.h"

#include <memory>
#include <D3Dcommon.h>

class CGameLogic;
class IRenderer;
class IRenderResource;
struct MeshVertex;
struct CXMFLOAT3;
enum SHADER_TYPE;

constexpr int MAX_MESH_NAME = 32;

class IMesh
{
public:
	virtual const TCHAR* GetName( ) const = 0;
	virtual void SetName( const TCHAR* name ) = 0;

	virtual void SetModelData( MeshVertex* pOrignal, UINT vertexCount ) = 0;
	virtual void SetIndexData( void* pOrignal, UINT indexCount ) = 0;
	virtual void SetColor( const CXMFLOAT3& color ) = 0;
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) = 0;

	virtual void Draw( CGameLogic& renderer ) = 0;

	virtual void SetMaterial( Material material ) = 0;
	virtual void SetTexture( RE_HANDLE pTexture ) = 0;
	virtual RE_HANDLE GetTexture( ) const = 0;

	virtual const void* GetMeshData( ) const = 0;
	virtual int GetVerticesCount( ) const = 0;

	virtual ~IMesh( ) = default;

protected:
	IMesh( ) = default;
};

