#pragma once

#include "BaseMesh.h"
#include <vector>

class ISurface;
class IShaderResource;

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
	IShaderResource* m_pTexture;
};

class CObjMesh : public BaseMesh
{
public:
	virtual bool Load( D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) override;
	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	void AddMaterialGroup( const ObjSurfaceTrait& trait );

	CObjMesh( );
	virtual ~CObjMesh( );

private:
	std::vector<ObjSurfaceTrait> m_mtlGroup;
};

