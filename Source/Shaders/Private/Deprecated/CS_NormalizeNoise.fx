#include "Common/Constants.fxh"

RWTexture3D<float> NoiseTex : register( u0 );
RWStructuredBuffer<int> NoiseMinMax : register( u1 );

static const float MinMaxScale = 10000000;

[numthreads(8, 8, 8)]
void main( uint3 DTid: SV_DispatchThreadID )
{
	uint3 dims;
	NoiseTex.GetDimensions( dims.x, dims.y, dims.z );
	
	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float minNoise = NoiseMinMax[0] / MinMaxScale;
		float maxNoise = NoiseMinMax[1] / MinMaxScale;
		float normalized = ( NoiseTex[DTid] - minNoise ) / ( maxNoise - minNoise );

		NoiseTex[DTid] = normalized;
	}
}