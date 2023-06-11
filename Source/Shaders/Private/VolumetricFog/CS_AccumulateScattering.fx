Texture3D<float4> FrustumVolume : register( t0 );

RWTexture3D<float4> AccumulatedVolume : register( u0 );

static const float DensityScale = 0.01f;

float4 ScatterStep( float3 accumulatedLight, float accumulatedTransmittance, float3 sliceLight, float sliceDensity, float numSlice )
{
	sliceDensity = max( sliceDensity, 0.000001f );
	float sliceTransmittance = exp( -sliceDensity * DensityScale );

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

	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float4 accum = float4( 0.f, 0.f, 0.f, 1.f );
		uint3 pos = uint3( DTid.xy, 0 );

		for ( uint z = 0; z < dims.z; ++z )
		{
			pos.z = z;
			float4 slice = FrustumVolume[pos];
			accum = ScatterStep( accum.rgb, accum.a, slice.rgb, slice.a, dims.z );
			AccumulatedVolume[pos] = accum;
		}
	}
}