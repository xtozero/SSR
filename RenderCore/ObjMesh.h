#pragma once

#include "BaseMesh.h"
#include <vector>

class ITexture;

struct ObjMaterialTrait
{
	UINT m_indexOffset;
	UINT m_indexCount;
	String m_textureName;
	std::shared_ptr<ITexture> m_pTexture;
};

class CObjMesh : public BaseMesh
{
public:
	virtual bool Load( ) override;
	virtual void Draw( ID3D11DeviceContext* pDeviceContext ) override;

	void AddMaterialGroup( const ObjMaterialTrait& trait );

	CObjMesh( );
	virtual ~CObjMesh( );

private:
	std::vector<ObjMaterialTrait> m_mtlGroup;
};

