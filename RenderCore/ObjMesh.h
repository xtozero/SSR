#pragma once

#include "BaseMesh.h"
#include <vector>

class IRenderResource;
class ISurface;

struct ObjSurfaceTrait
{
	ObjSurfaceTrait() : m_indexOffset( 0 ),
		m_indexCount( 0 ),
		m_pSurface( nullptr ),
		m_pTexture( nullptr )
	{}

	UINT m_indexOffset;
	UINT m_indexCount;
	ISurface* m_pSurface;
	IRenderResource* m_pTexture;
};

class CObjMesh : public BaseMesh
{
public:
	virtual bool Load( IRenderer& renderer, UINT primitive = RESOURCE_PRIMITIVE::TRIANGLELIST ) override;
	virtual void Draw( IRenderer& renderer ) override;

	void AddMaterialGroup( const ObjSurfaceTrait& trait );

	CObjMesh( );
	virtual ~CObjMesh( );

private:
	std::vector<ObjSurfaceTrait> m_mtlGroup;
};

