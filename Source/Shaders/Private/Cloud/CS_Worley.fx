#include "Cloud/Noise.fxh"

RWTexture3D<float4> NoiseTex : register( u0 );

float4 Worley( float3 uvw )
{
	float cellCount = 2.f;
	float worleyNoise0 = ( 1.f - WorleyNoise3D( uvw, cellCount * 1.f ) );
	float worleyNoise1 = ( 1.f - WorleyNoise3D( uvw, cellCount * 2.f ) );
	float worleyNoise2 = ( 1.f - WorleyNoise3D( uvw, cellCount * 4.f ) );
	float worleyNoise3 = ( 1.f - WorleyNoise3D( uvw, cellCount * 8.f ) );

	float worleyFBM0 = worleyNoise0 * 0.625f + worleyNoise1 * 0.25f + worleyNoise2 * 0.125f;
	float worleyFBM1 = worleyNoise1 * 0.625f + worleyNoise2 * 0.25f + worleyNoise3 * 0.125f;
	float worleyFBM2 = worleyNoise2 * 0.75f + worleyNoise3 * 0.25f;

	return float4( worleyFBM0, worleyFBM1, worleyFBM2, 1.f );
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint3 dims;
	NoiseTex.GetDimensions( dims.x, dims.y, dims.z );

	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float3 samplePoint = float3( DTid ) / dims;
		float4 noise = Worley( samplePoint );
		NoiseTex[DTid] = noise;
	}
}