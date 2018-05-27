#pragma once

#include "Aaboundingbox.h"
#include "Math/Util.h"

class CFrustum
{
public:
	enum
	{
		X_MAX = 0x00000001,
		Y_MAX = 0x00000002,
		Z_MAX = 0x00000004,
	};

	explicit CFrustum( const CXMFLOAT4X4& projectionMat )
	{
		CXMFLOAT4 column[4] = {
			{ projectionMat( 0, 0 ), projectionMat( 1, 0 ), projectionMat( 2, 0 ), projectionMat( 3, 0 ) },
			{ projectionMat( 0, 1 ), projectionMat( 1, 1 ), projectionMat( 2, 1 ), projectionMat( 3, 1 ) },
			{ projectionMat( 0, 2 ), projectionMat( 1, 2 ), projectionMat( 2, 2 ), projectionMat( 3, 2 ) },
			{ projectionMat( 0, 3 ), projectionMat( 1, 3 ), projectionMat( 2, 3 ), projectionMat( 3, 3 ) }
		};

		// http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
		// Plane Extraction in Direct3D
		m_plane[0] = column[3] + column[0]; // left
		m_plane[1] = column[3] - column[0]; // right
		m_plane[2] = column[3] + column[1]; // bottom
		m_plane[3] = column[3] - column[1]; // top
		m_plane[4] = column[2];			    // near
		m_plane[5] = column[3] - column[2]; // far

		for ( auto& plane : m_plane )
		{
			plane = DirectX::XMPlaneNormalize( plane );
		}

		// http://old.cescg.org/CESCG-2002/DSykoraJJelinek/
		// Table 1: Look-up table for determining the n and p-vertices.
		for ( int i = 0; i < 6; ++i )
		{
			m_vertexLUT[i] = ( ( m_plane[i].x > 0 ) ? X_MAX : 0 ) | 
							( ( m_plane[i].y > 0 ) ? Y_MAX : 0 ) | 
							( ( m_plane[i].z > 0 ) ? Z_MAX : 0 );
		}

		for ( int i = 0; i < 8; ++i )
		{
			const CXMFLOAT4& p0 = ( i & 1 ) ? m_plane[4] : m_plane[5];
			const CXMFLOAT4& p1 = ( i & 2 ) ? m_plane[3] : m_plane[2];
			const CXMFLOAT4& p2 = ( i & 4 ) ? m_plane[0] : m_plane[1];

			PlanesIntersection( p0, p1, p2, m_vertices[i] );
		}
	}

	using LookUpTable = int[6];
	const LookUpTable& GetVertexLUT( ) const { return m_vertexLUT; }

	const CXMFLOAT4( &GetPlanes( ) const )[6]{ return m_plane; }

private:
	CXMFLOAT4 m_plane[6];
	CXMFLOAT3 m_vertices[8];
	int m_vertexLUT[6];
};