#pragma once

#include "BaseMesh.h"
#include <vector>

class IRenderResource;
struct Surface;

struct ObjSurfaceTrait
{
	UINT m_indexOffset = 0;
	UINT m_indexCount = 0;
	Surface* m_pSurface = nullptr;
	RE_HANDLE m_texture = RE_HANDLE_TYPE::INVALID_HANDLE;
};

class CObjMesh : public BaseMesh
{
public:
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;
	virtual void Draw( CGameLogic& gameLogic ) override;

	void AddMaterialGroup( const ObjSurfaceTrait& trait );

	CObjMesh( );
	virtual ~CObjMesh( );

private:
	std::vector<ObjSurfaceTrait> m_mtlGroup;
};

