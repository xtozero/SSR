#pragma once

#include "CommonMeshDefine.h"
#include "IModelLoader.h"

#include <map>
#include <memory>
#include <vector>

class IMesh;

class CObjMeshLoader : public IModelLoader
{
public:
	virtual Owner<IMesh*> LoadMeshFromFile( IRenderer& renderer, const char* pFileName, SurfaceMap& pSurfaceManager ) override;

private:
	void Initialize( );
	std::vector<MeshVertex> BuildVertices( );
	void LoadMaterialFile( const char* pFileName, SurfaceMap& surface );

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
		ObjFaceMtlInfo( UINT endFaceIndex, const std::string& materialName ) :
		m_endFaceIndex( endFaceIndex ),
		m_materialName( materialName )
		{}

		UINT m_endFaceIndex;
		std::string m_materialName;
	};

	std::vector<ObjFaceMtlInfo> m_faceMtlGroup;

	struct ObjMtlInfo
	{
		ObjMtlInfo( UINT startIndex, UINT endIndex, const std::string& textureName ) :
			m_startIndex( startIndex ),
			m_endIndex( endIndex ),
			m_materialName( textureName )
		{
		}

		UINT m_startIndex;
		UINT m_endIndex;
		std::string m_materialName;
	};

	std::vector<ObjMtlInfo> m_mtlGroup;

	struct ObjRawMtlInfo
	{
		std::string m_textureName;
	};
};

