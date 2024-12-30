#pragma once

#include "Math/Vector.h"
#include "Math/Vector2.h"
#include "NameTypes.h"
#include "SizedTypes.h"

#include <string>
#include <vector>

namespace rendercore
{
	struct MeshVertexInstance final
	{
		MeshVertexInstance( int32 positionId, int32 normalId, int32 texCoordId ) :
			m_positionId( positionId ), m_normalId( normalId ), m_texCoordId( texCoordId ) {}

		friend bool operator<( const MeshVertexInstance& lhs, const MeshVertexInstance& rhs )
		{
			if ( lhs.m_positionId == rhs.m_positionId )
			{
				if ( lhs.m_normalId == rhs.m_normalId )
				{
					return lhs.m_texCoordId < rhs.m_texCoordId;
				}

				return lhs.m_normalId < rhs.m_normalId;
			}

			return lhs.m_positionId < rhs.m_positionId;
		}

		int32 m_positionId = -1;
		int32 m_normalId = -1;
		int32 m_texCoordId = -1;
	};

	struct MeshTriangle final
	{
		uint32 m_vertexInstanceId[3];
	};

	class Meshlet final
	{
	public:
		Meshlet( uint32 vertexOffset, uint32 triangleOffset, uint32 vertexCount, uint32 triangleCount, const Vector& center, float radius )
			: m_vertexOffset( vertexOffset )
			, m_triangleOffset( triangleOffset )
			, m_vertexCount( vertexCount )
			, m_triangleCount( triangleCount )
			, m_center( center )
			, m_radius( radius )
		{}
		Meshlet() = default;

		friend Archive& operator<<( Archive& ar, Meshlet& meshlet )
		{
			ar << meshlet.m_vertexOffset
				<< meshlet.m_triangleOffset
				<< meshlet.m_vertexCount
				<< meshlet.m_triangleCount
				<< meshlet.m_center
				<< meshlet.m_radius;

			return ar;
		}

		uint32 m_vertexOffset = 0;
		uint32 m_triangleOffset = 0;

		uint32 m_vertexCount = 0;
		uint32 m_triangleCount = 0;

		Vector m_center = Vector::ZeroVector;
		float m_radius = 0;
	};

	struct MeshPolygon final
	{
		std::vector<uint32> m_triangleId;
		std::vector<Meshlet> m_meshlets;
	};

	struct MeshDescription final
	{
		std::vector<Vector> m_positions;
		std::vector<Vector> m_normals;
		std::vector<Vector2> m_texCoords;

		std::vector<MeshVertexInstance> m_vertexInstances;
		std::vector<MeshTriangle> m_triangles;
		std::vector<MeshPolygon> m_polygons;
		std::vector<Name> m_polygonMaterialName;

		std::vector<uint32> m_meshletVertices;
		std::vector<uint32> m_meshletTriangles;
	};
}
