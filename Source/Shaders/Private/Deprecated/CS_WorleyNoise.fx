#include "Common/Constants.fxh"

StructuredBuffer<float3> SeedPoints : register( t0 );
RWTexture3D<float> NoiseTex : register( u0 );
RWStructuredBuffer<int> NoiseMinMax : register( u1 );

uint NumDivisions;
bool Invert;

static const int3 GridOffset[27] = {
	// Top
	int3( -1, 1, -1 ),
	int3( 0, 1, -1 ),
	int3( 1, 1, -1 ),
	int3( -1, 1, 0 ),
	int3( 0, 1, 0 ),
	int3( 1, 1, 0 ),
	int3( -1, 1, 1 ),
	int3( 0, 1, 1 ),
	int3( 1, 1, 1 ),

	// Mid
	int3( -1, 0, -1 ),
	int3( 0, 0, -1 ),
	int3( 1, 0, -1 ),
	int3( -1, 0, 0 ),
	int3( 0, 0, 0 ),
	int3( 1, 0, 0 ),
	int3( -1, 0, 1 ),
	int3( 0, 0, 1 ),
	int3( 1, 0, 1 ),

	// Bottom
	int3( -1, -1, -1 ),
	int3( 0, -1, -1 ),
	int3( 1, -1, -1 ),
	int3( -1, -1, 0 ),
	int3( 0, -1, 0 ),
	int3( 1, -1, 0 ),
	int3( -1, -1, 1 ),
	int3( 0, -1, 1 ),
	int3( 1, -1, 1 ),
};

static const float MinMaxScale = 10000000;

bool IsWarp( int3 gridId )
{
	int minId = min( gridId.x, min( gridId.y, gridId.z ) );
	int maxId = max( gridId.x, max( gridId.y, gridId.z ) );

	return minId == -1 || maxId == (int)NumDivisions;
}

float3 WarpedPoint( float3 seedPoint, int3 neighbor, int3 gridOffset )
{
	float3 offset = float3(
		neighbor.x == -1 || neighbor.x == (int)NumDivisions ? gridOffset.x : 0.f,
		neighbor.y == -1 || neighbor.y == (int)NumDivisions ? gridOffset.y : 0.f,
		neighbor.z == -1 || neighbor.z == (int)NumDivisions ? gridOffset.z : 0.f
	);

	return seedPoint + offset;
}

float Werley( float3 samplePoint )
{
	int3 gridId = floor( samplePoint * NumDivisions );

	float d = FLT_MAX;
	for ( int i = 0; i < 27; ++i )
	{
		int3 neighbor = gridId + GridOffset[i];
		if ( IsWarp( neighbor ) )
		{
			int3 wrapedGridId = neighbor + NumDivisions;
			wrapedGridId %= NumDivisions;

			uint seedIdx = wrapedGridId.x + NumDivisions * ( wrapedGridId.y + NumDivisions * wrapedGridId.z );
			/*for ( int j = 0; j < 27; ++j )
			{
				d = min( d, distance( samplePoint, SeedPoints[seedIdx] + float3( GridOffset[j] ) ) );
			}*/
			float3 warpedPoint = WarpedPoint( SeedPoints[seedIdx], neighbor, GridOffset[i] );
			d = min( d, distance( samplePoint, warpedPoint ) );
		}
		else
		{
			uint seedIdx = neighbor.x + NumDivisions * ( neighbor.y + NumDivisions * neighbor.z );
			d = min( d, distance( samplePoint, SeedPoints[seedIdx] ) );
		}
	}

	return Invert ? 1.f - d : d;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint3 dims;
	NoiseTex.GetDimensions( dims.x, dims.y, dims.z );
	
	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float3 samplePoint = DTid / (float3)dims;
		float noise = Werley( samplePoint );
		NoiseTex[DTid] = noise;

		int val = noise * MinMaxScale;
		InterlockedMin( NoiseMinMax[0], val );
		InterlockedMax( NoiseMinMax[1], val );
	}
}