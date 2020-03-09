SamplerState baseSampler : register( s0 );
Texture2D baseTexture : register( t0 );

float4 Sample( float2 texcoord )
{
	return baseTexture.Sample( baseSampler, texcoord );
}

float random( float3 seed )
{
    return frac(sin(dot(seed, float3(53.1215, 21.1352, 9.1322))) * 43758.5453);
}

float2 PoissonSample4x4( int i, int j )
{
	float2 offset = float2( ( i - 2.f ), ( j - 2.f ) );

	return offset;
}

float2 RotatePoissonSample4x4( float sin, float cos, int i, int j )
{
	float2 offset = float2( ( i - 2.f ), ( j - 2.f ) );
	offset = float2( offset.x * cos - offset.y * sin , offset.x * sin + offset.y * cos );
	
	return offset;
}