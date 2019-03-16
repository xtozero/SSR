#pragma once

#include "CommonMeshDefine.h"

#include <vector>
#include <windef.h>

struct MeshData 
{
	std::vector<MeshVertex> m_vertices;
	std::vector<WORD> m_indices;
};

namespace MeshBuildHelper
{
	inline void Subdivide( MeshData& meshData )
	{
		using namespace DirectX;

		MeshData copy = meshData;

		int numTris = meshData.m_indices.size( ) / 3;

		meshData.m_vertices.clear( );
		meshData.m_indices.clear( );

		meshData.m_vertices.reserve( numTris * 6 );
		meshData.m_indices.reserve( numTris * 4 );

		for ( int i = 0; i < numTris; ++i )
		{
			const CXMFLOAT3& v0 = copy.m_vertices[copy.m_indices[i * 3 + 0]].m_position;
			const CXMFLOAT3& v1 = copy.m_vertices[copy.m_indices[i * 3 + 0]].m_position;
			const CXMFLOAT3& v2 = copy.m_vertices[copy.m_indices[i * 3 + 0]].m_position;

			CXMFLOAT3 m0 = 0.5f * ( v0 + v1 );
			CXMFLOAT3 m1 = 0.5f * ( v1 + v2 );
			CXMFLOAT3 m2 = 0.5f * ( v0 + v2 );

			meshData.m_vertices.emplace_back( v0 );
			meshData.m_vertices.emplace_back( v1 );
			meshData.m_vertices.emplace_back( v2 );
			meshData.m_vertices.emplace_back( m0 );
			meshData.m_vertices.emplace_back( m1 );
			meshData.m_vertices.emplace_back( m2 );

			meshData.m_indices.push_back( i * 6 + 0 );
			meshData.m_indices.push_back( i * 6 + 3 );
			meshData.m_indices.push_back( i * 6 + 5 );

			meshData.m_indices.push_back( i * 6 + 3 );
			meshData.m_indices.push_back( i * 6 + 4 );
			meshData.m_indices.push_back( i * 6 + 5 );

			meshData.m_indices.push_back( i * 6 + 5 );
			meshData.m_indices.push_back( i * 6 + 4 );
			meshData.m_indices.push_back( i * 6 + 2 );

			meshData.m_indices.push_back( i * 6 + 3 );
			meshData.m_indices.push_back( i * 6 + 1 );
			meshData.m_indices.push_back( i * 6 + 4 );
		}
	}

	inline MeshData CreateSphere( float radius, UINT sliceCount, UINT stackCount )
	{
		MeshData meshData;

		float phiStep = DirectX::XM_PI / stackCount;
		float thetaStep = DirectX::XM_PI * 2.f / sliceCount;

		// top
		meshData.m_vertices.emplace_back( CXMFLOAT3( 0.f, radius, 0.f ), CXMFLOAT3( 0.f, 1.f, 0.f ), CXMFLOAT2( 0.f, 0.f ) );

		for ( UINT y = 1; y < stackCount; ++y )
		{
			float phi = y * phiStep;

			for ( UINT x = 0; x <= sliceCount; ++x )
			{
				float theta = x * thetaStep;

				CXMFLOAT3 pos( sinf( phi ) * cosf( theta ), cosf( phi ), sinf( phi ) * sinf( theta ) );
				pos *= radius;

				CXMFLOAT3 normal = XMVector3Normalize( pos );

				CXMFLOAT2 uv( static_cast<float>( x ) / sliceCount, static_cast<float>( y ) / stackCount );

				meshData.m_vertices.emplace_back( pos, normal, uv );
			}
		}

		// bottom
		meshData.m_vertices.emplace_back( CXMFLOAT3( 0.f, -radius, 0.f ), CXMFLOAT3( 0.f, -1.f, 0.f ), CXMFLOAT2( 0.f, 1.f ) );

		for ( UINT x = 1; x <= sliceCount; ++x )
		{
			meshData.m_indices.push_back( 0 );
			meshData.m_indices.push_back( x + 1 );
			meshData.m_indices.push_back( x );
		}

		UINT baseIndex = 1;
		UINT ringVertexCount = sliceCount + 1;

		for ( UINT y = 0, end = stackCount - 2; y < end; ++y )
		{
			int curStackStart = y * ringVertexCount;
			int nextStackStart = ( y + 1 ) * ringVertexCount;

			for ( UINT x = 0; x < sliceCount; ++x )
			{
				meshData.m_indices.push_back( baseIndex + curStackStart + x );
				meshData.m_indices.push_back( baseIndex + curStackStart + x + 1 );
				meshData.m_indices.push_back( baseIndex + nextStackStart + x );

				meshData.m_indices.push_back( baseIndex + curStackStart + x + 1 );
				meshData.m_indices.push_back( baseIndex + nextStackStart + x + 1 );
				meshData.m_indices.push_back( baseIndex + nextStackStart + x );
			}
		}

		UINT southPoleIndex = meshData.m_vertices.size( ) - 1;
		baseIndex = southPoleIndex - ringVertexCount;

		for ( UINT x = 0; x < sliceCount; ++x )
		{
			meshData.m_indices.push_back( southPoleIndex );
			meshData.m_indices.push_back( baseIndex + x );
			meshData.m_indices.push_back( baseIndex + x + 1 );
		}

		return meshData;
	}

	inline MeshData CreateBox( float width, float height, float depth )
	{
		MeshData meshData;

		float w2 = 0.5f * width;
		float h2 = 0.5f * height;
		float d2 = 0.5f * depth;

		std::vector<MeshVertex>& vertices = meshData.m_vertices;
		vertices.reserve( 24 );

		// front
		vertices.emplace_back( CXMFLOAT3( -w2, -h2, -d2 ), CXMFLOAT3( 0.f, 0.f, -1.f ), CXMFLOAT2( 0.f, 1.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, h2, -d2 ), CXMFLOAT3( 0.f, 0.f, -1.f ), CXMFLOAT2( 0.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, h2, -d2 ), CXMFLOAT3( 0.f, 0.f, -1.f ), CXMFLOAT2( 1.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, -h2, -d2 ), CXMFLOAT3( 0.f, 0.f, -1.f ), CXMFLOAT2( 1.f, 1.f ) );

		// back
		vertices.emplace_back( CXMFLOAT3( w2, -h2, d2 ), CXMFLOAT3( 0.f, 0.f, 1.f ), CXMFLOAT2( 0.f, 1.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, h2, d2 ), CXMFLOAT3( 0.f, 0.f, 1.f ), CXMFLOAT2( 0.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, h2, d2 ), CXMFLOAT3( 0.f, 0.f, 1.f ), CXMFLOAT2( 1.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, -h2, d2 ), CXMFLOAT3( 0.f, 0.f, 1.f ), CXMFLOAT2( 1.f, 1.f ) );
		
		// left
		vertices.emplace_back( CXMFLOAT3( -w2, -h2, d2 ), CXMFLOAT3( -1.f, 0.f, 0.f ), CXMFLOAT2( 0.f, 1.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, h2, d2 ), CXMFLOAT3( -1.f, 0.f, 0.f ), CXMFLOAT2( 0.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, h2, -d2 ), CXMFLOAT3( -1.f, 0.f, 0.f ), CXMFLOAT2( 1.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, -h2, -d2 ), CXMFLOAT3( -1.f, 0.f, 0.f ), CXMFLOAT2( 1.f, 1.f ) );
		
		// right
		vertices.emplace_back( CXMFLOAT3( w2, -h2, -d2 ), CXMFLOAT3( 1.f, 0.f, 0.f ), CXMFLOAT2( 0.f, 1.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, h2, -d2 ), CXMFLOAT3( 1.f, 0.f, 0.f ), CXMFLOAT2( 0.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, h2, d2 ), CXMFLOAT3( 1.f, 0.f, 0.f ), CXMFLOAT2( 1.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, -h2, d2 ), CXMFLOAT3( 1.f, 0.f, 0.f ), CXMFLOAT2( 1.f, 1.f ) );
		
		// top
		vertices.emplace_back( CXMFLOAT3( -w2, h2, -d2 ), CXMFLOAT3( 0.f, 1.f, 0.f ), CXMFLOAT2( 0.f, 1.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, h2, d2 ), CXMFLOAT3( 0.f, 1.f, 0.f ), CXMFLOAT2( 0.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, h2, d2 ), CXMFLOAT3( 0.f, 1.f, 0.f ), CXMFLOAT2( 1.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, h2, -d2 ), CXMFLOAT3( 0.f, 1.f, 0.f ), CXMFLOAT2( 1.f, 1.f ) );
		
		// bottom
		vertices.emplace_back( CXMFLOAT3( -w2, -h2, d2 ), CXMFLOAT3( 0.f, -1.f, 0.f ), CXMFLOAT2( 0.f, 1.f ) );
		vertices.emplace_back( CXMFLOAT3( -w2, -h2, -d2 ), CXMFLOAT3( 0.f, -1.f, 0.f ), CXMFLOAT2( 0.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, -h2, -d2 ), CXMFLOAT3( 0.f, -1.f, 0.f ), CXMFLOAT2( 1.f, 0.f ) );
		vertices.emplace_back( CXMFLOAT3( w2, -h2, d2 ), CXMFLOAT3( 0.f, -1.f, 0.f ), CXMFLOAT2( 1.f, 1.f ) );

		meshData.m_indices.resize( 36 );

		// front
		meshData.m_indices[0] = 0;	meshData.m_indices[1] = 1;	meshData.m_indices[2] = 2;
		meshData.m_indices[3] = 0;	meshData.m_indices[4] = 2;	meshData.m_indices[5] = 3;
		
		// back
		meshData.m_indices[6] = 4;	meshData.m_indices[7] = 5;	meshData.m_indices[8] = 6;
		meshData.m_indices[9] = 4;	meshData.m_indices[10] = 6;	meshData.m_indices[11] = 7;

		// left
		meshData.m_indices[12] = 8;	meshData.m_indices[13] = 9;	meshData.m_indices[14] = 10;
		meshData.m_indices[15] = 8;	meshData.m_indices[16] = 10;	meshData.m_indices[17] = 11;

		// right
		meshData.m_indices[18] = 12;	meshData.m_indices[19] = 13;	meshData.m_indices[20] = 14;
		meshData.m_indices[21] = 12;	meshData.m_indices[22] = 14;	meshData.m_indices[23] = 15;

		// top
		meshData.m_indices[24] = 16;	meshData.m_indices[25] = 17;	meshData.m_indices[26] = 18;
		meshData.m_indices[27] = 16;	meshData.m_indices[28] = 18;	meshData.m_indices[29] = 19;

		// bottom
		meshData.m_indices[30] = 20;	meshData.m_indices[31] = 21;	meshData.m_indices[32] = 22;
		meshData.m_indices[33] = 20;	meshData.m_indices[34] = 22;	meshData.m_indices[35] = 23;

		return meshData;
	}

	inline MeshData CreateCylinder( float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount )
	{
		MeshData meshData;

		float stackHeight = height / stackCount;
		float radiusStep = ( bottomRadius - topRadius ) / stackCount;
		UINT ringcount = stackCount + 1;
		float dTheta = DirectX::XM_2PI / sliceCount;

		// top center;
		meshData.m_vertices.emplace_back( CXMFLOAT3( 0.f, height * 0.5f, 0.f ), CXMFLOAT3( 0.f, 1.f, 0.f ), CXMFLOAT2( 0.f, 0.f ) );

		for ( UINT i = 0; i < ringcount; ++i )
		{
			float y = ( height * 0.5f ) - ( i * stackHeight );
			float r = topRadius + ( i * radiusStep );
			float theta = 0;

			for ( UINT j = 0; j <= static_cast<int>( sliceCount ); ++j )
			{
				theta += dTheta;
				float cost = cos( theta );
				float sint = sin( theta );

				CXMFLOAT3 pos( r * cost, y, r * sint );
				CXMFLOAT3 normal = pos;
				normal.y = 0;
				normal = XMVector3Normalize( normal );
				CXMFLOAT2 uv( static_cast<float>( j ) / sliceCount, 1.f - static_cast<float>( i ) / stackCount );

				meshData.m_vertices.emplace_back( pos, normal, uv );
			}
		}

		// bottom center;
		meshData.m_vertices.emplace_back( CXMFLOAT3( 0.f, height * -0.5f, 0.f ), CXMFLOAT3( 0.f, -1.f, 0.f ), CXMFLOAT2( 0.f, 1.f ) );

		for ( UINT x = 1; x <= sliceCount; ++x )
		{
			meshData.m_indices.push_back( 0 );
			meshData.m_indices.push_back( x + 1 );
			meshData.m_indices.push_back( x );
		}

		UINT baseIndex = 1;

		for ( UINT y = 0, end = stackCount; y < end; ++y )
		{
			int curStackStart = y * ringcount;
			int nextStackStart = (y + 1) * ringcount;

			for ( UINT x = 0; x < sliceCount; ++x )
			{
				meshData.m_indices.push_back( baseIndex + curStackStart + x );
				meshData.m_indices.push_back( baseIndex + curStackStart + x + 1 );
				meshData.m_indices.push_back( baseIndex + nextStackStart + x );

				meshData.m_indices.push_back( baseIndex + curStackStart + x + 1 );
				meshData.m_indices.push_back( baseIndex + nextStackStart + x + 1 );
				meshData.m_indices.push_back( baseIndex + nextStackStart + x );
			}
		}

		UINT southPoleIndex = meshData.m_vertices.size( ) - 1;
		baseIndex = southPoleIndex - ringcount;

		for ( UINT x = 0; x <= sliceCount; ++x )
		{
			meshData.m_indices.push_back( southPoleIndex );
			meshData.m_indices.push_back( baseIndex + x );
			meshData.m_indices.push_back( baseIndex + x + 1 );
		}

		return meshData;
	}

	inline MeshData CreateGeoshpere( float radius, UINT subdivisions )
	{
		MeshData meshData;

		constexpr float x = 0.525731f;
		constexpr float z = 0.850651f;

		meshData.m_vertices.resize( 12 );

		CXMFLOAT3 pos[12] =
		{
			CXMFLOAT3( -x, 0.f, z ), CXMFLOAT3( x, 0.f ,z ),
			CXMFLOAT3( -x, 0.f, -z ), CXMFLOAT3( x, 0.f, -z ),
			CXMFLOAT3( 0.f, z, x ),	CXMFLOAT3( 0.f, z, -x ),
			CXMFLOAT3( 0.f, -z, x ), CXMFLOAT3( 0.f, -z, -x ),
			CXMFLOAT3( z, x, 0.f ), CXMFLOAT3( -z, x, 0.f ),
			CXMFLOAT3( z, -x, 0.f ), CXMFLOAT3( -z, -x, 0.f ),
		};

		for ( int i = 0; i < 12; ++i )
		{
			meshData.m_vertices[i].m_position = pos[i];
		}

		meshData.m_indices.resize( 60 );

		WORD idx[60] = {
			1, 4, 0, 4, 9, 0, 4, 5, 9, 8, 5, 4, 1, 8, 4,
			1, 10, 8, 10, 3, 8, 8, 3, 5, 3, 2, 5, 3, 7, 2,
			3, 10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6, 1, 0,
			10, 1, 6, 11, 0, 9, 2, 11, 9, 5, 2, 9, 11, 2, 7
		};

		meshData.m_indices.assign( &idx[0], &idx[60] );

		for ( UINT i = 0; i < subdivisions; ++i )
		{
			Subdivide( meshData );
		}

		for ( auto& vertex : meshData.m_vertices )
		{
			CXMFLOAT3 n = XMVector3Normalize( vertex.m_position );

			vertex.m_normal = n;
			vertex.m_position = n * radius;

			float phi = atan2( vertex.m_position.z, vertex.m_position.x );
			if ( phi < 0.f )
			{
				phi += DirectX::XM_2PI;
			}

			float theta = acosf( vertex.m_position.y / radius );

			vertex.m_texcoord.x = theta / DirectX::XM_PI;
			vertex.m_texcoord.y = phi / DirectX::XM_2PI;
		}

		return meshData;
	}

	inline MeshData CreateGrid( float width, float depth, UINT m, UINT n )
	{
		MeshData meshData;

		float halfWidth = width * 0.5f;
		float halfDepth = depth * 0.5f;

		float du = 1.f / ( n - 1 );
		float dv = 1.f / ( m - 1 );

		float dx = width * du;
		float dz = depth * dv;

		meshData.m_vertices.resize( m * n );

		for ( int i = 0; i < m; ++i )
		{
			float z = halfDepth - i * dz;
			for ( int j = 0; j < n; ++j )
			{
				float x = -halfWidth + j * dx;

				MeshVertex& vertex = meshData.m_vertices[i*n + j];
				vertex.m_position = CXMFLOAT3( x, 0.f, z );
				vertex.m_normal = CXMFLOAT3( 0.f, 1.f, 0.f );
				vertex.m_texcoord = CXMFLOAT2( j * du, i * dv );
			}
		}

		meshData.m_indices.resize( ( m - 1 ) * ( n - 1 ) * 6 );

		int k = 0;
		for ( int i = 0, iEnd = m - 1; i < iEnd; ++i )
		{
			for ( int j = 0, jEnd = n - 1; j < jEnd; ++j )
			{
				meshData.m_indices[k++] = i * n + j;
				meshData.m_indices[k++] = i * n + j + 1;
				meshData.m_indices[k++] = (i + 1) * n + j;

				meshData.m_indices[k++] = ( i + 1 ) * n + j;
				meshData.m_indices[k++] = i * n + j + 1;
				meshData.m_indices[k++] = ( i + 1 ) * n + j + 1;
			}
		}

		return meshData;
	}
}