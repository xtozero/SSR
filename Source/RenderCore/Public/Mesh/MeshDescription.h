#pragma once

#include "Math/Vector.h"
#include "Math/Vector2.h"
#include "SizedTypes.h"

#include <string>
#include <vector>

struct MeshVertexInstance
{
	MeshVertexInstance( int32 positionID, int32 normalID, int32 texCoordID ) :
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

	int32 m_positionID = -1;
	int32 m_normalID = -1;
	int32 m_texCoordID = -1;
};

struct MeshTriangle
{
	size_t m_vertexInstanceID[3];
};

struct MeshPolygon
{
	std::vector<size_t> m_triangleID;
};

struct MeshDescription
{
	std::vector<Vector> m_positions;
	std::vector<Vector> m_normals;
	std::vector<Vector2> m_texCoords;

	std::vector<MeshVertexInstance> m_vertexInstances;
	std::vector<MeshTriangle> m_triangles;
	std::vector<MeshPolygon> m_polygons;
	std::vector<std::string> m_polygonMaterialName;
};