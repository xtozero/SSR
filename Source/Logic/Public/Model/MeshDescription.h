#pragma once

#include "Math/CXMFloat.h"

#include <string>
#include <vector>

struct MeshVertexInstance
{
	MeshVertexInstance( int positionID, int normalID, int texCoordID ) :
		m_positionID( positionID ), m_normalID( normalID ), m_texCoordID( texCoordID ) {}

	friend bool operator<( const MeshVertexInstance& lhs, const MeshVertexInstance& rhs )
	{
		if ( lhs.m_positionID == rhs.m_positionID )
		{
			if ( lhs.m_normalID == rhs.m_normalID )
			{
				return lhs.m_texCoordID < rhs.m_texCoordID;
			}

			return lhs.m_normalID < rhs.m_normalID;
		}

		return lhs.m_positionID < rhs.m_positionID;
	}

	int m_positionID;
	int m_normalID;
	int m_texCoordID;
};

struct MeshTriangle
{
	std::size_t m_vertexInstanceID[3];
};

struct MeshPolygon
{
	std::vector<std::size_t> m_triangleID;
};

struct MeshDescription
{
	std::vector<CXMFLOAT3> m_positions;
	std::vector<CXMFLOAT3> m_normals;
	std::vector<CXMFLOAT2> m_texCoords;

	std::vector<MeshVertexInstance> m_vertexInstances;
	std::vector<MeshTriangle> m_triangles;
	std::vector<MeshPolygon> m_polygons;
	std::vector<std::string> m_polygonMaterialName;
};