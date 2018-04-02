#pragma once

#include "IModelLoader.h"
#include "CommonMeshDefine.h"

#include <map>
#include <memory>
#include <vector>

class IMesh;

class CObjMeshLoader : public IModelLoader
{
public:
	virtual Owner<IMesh*> LoadMeshFromFile( IRenderer& renderer, const TCHAR* pFileName, SurfaceMap& pSurfaceManager ) override;

private:
	void Initialize( );
	std::vector<MeshVertex> BuildVertices( );
	void LoadMaterialFile( const TCHAR* pFileName, SurfaceMap& surface );

private:
	void CalcObjNormal( );

	struct ObjFaceInfo
	{
		ObjFaceInfo( int position, int texCoord, int normal ) :
		m_position( position ),
		m_texCoord( texCoord ),
		m_normal( normal )
		{}

		int m_position;
		int m_texCoord;
		int m_normal;
	};

	std::vector<CXMFLOAT3> m_positions;
	std::vector<CXMFLOAT3> m_normals;
	std::vector<CXMFLOAT2> m_texCoords;

	std::vector<ObjFaceInfo> m_faceInfo;

	struct ObjFaceMtlInfo
	{
		ObjFaceMtlInfo( UINT endFaceIndex, const String& materialName ) :
		m_endFaceIndex( endFaceIndex ),
		m_materialName( materialName )
		{}

		UINT m_endFaceIndex;
		String m_materialName;
	};

	std::vector<ObjFaceMtlInfo> m_faceMtlGroup;

	struct ObjMtlInfo
	{
		ObjMtlInfo( UINT startIndex, UINT endIndex, const String& textureName ) :
			m_startIndex( startIndex ),
			m_endIndex( endIndex ),
			m_materialName( textureName )
		{
		}

		UINT m_startIndex;
		UINT m_endIndex;
		String m_materialName;
	};

	std::vector<ObjMtlInfo> m_mtlGroup;

	struct ObjRawMtlInfo
	{
		String m_textureName;
	};
};

