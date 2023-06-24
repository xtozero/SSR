#include "VolumetricFog/VolumetricFogCommon.fxh"

Texture3D<float4> FrustumVolume : register( t0 );

RWTexture3D<float4> AccumulatedVolume : register( u0 );

static const float DensityScale = 0.01f;

float4 ScatterStep( float3 accumulatedLight, float accumulatedTransmittance, float3 sliceLight, float sliceDensity, float tickness )
{
	sliceDensity = max( sliceDensity, 0.000001f );
	sliceDensity *= DensityScale;
	float sliceTransmittance = exp( -sliceDensity * tickness );

	float3 sliceLightIntegral = sliceLight * ( 1.f - sliceTransmittance ) / sliceDensity;

	accumulatedLight += sliceLightIntegral * accumulatedTransmittance;
	accumulatedTransmittance *= sliceTransmittance;

	return float4( accumulatedLight, accumulatedTransmittance );
}

[numthreads( 8, 8, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint3 dims;
	AccumulatedVolume.GetDimensions( dims.x, dims.y, dims.z );

	if ( all( DTid < dims ) )
	{
		float4 accum = float4( 0.f, 0.f, 0.f, 1.f );
		uint3 pos = uint3( DTid.xy, 0 );

		[loop]
		for ( uint z = 0; z < dims.z; ++z )
		{
			pos.z = z;
			float4 slice = FrustumVolume[pos];
			float tickness = SliceTickness( (float)z / dims.z, dims.z );

			accum = ScatterStep( accum.rgb, accum.a, slice.rgb, slice.a, tickness );
			AccumulatedVolume[pos] = accum;
		}
	}
}