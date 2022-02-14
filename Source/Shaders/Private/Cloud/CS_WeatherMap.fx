RWTexture2D<float4> WeatherTex : register( u0 );

float Random2D( float2 v )
{
	return frac( sin( dot( v.xy, float2( 12.9898, 78.233 ) ) ) * 43758.5453123 );
}

float Noise( float2 uv, float size )
{
	float2 grid = uv * size;

	float2 pi = floor( grid );
	float2 pf = frac( grid );

	float p0 = Random2D( pi );
	float p1 = Random2D( pi + float2( 1.f, 0.f ) );
	float p2 = Random2D( pi + float2( 0.f, 1.f ) );
	float p3 = Random2D( pi + float2( 1.f, 1.f ) );

	float2 weights = smoothstep( float2( 0.f, 0.f ), float2( 1.f, 1.f ), pf );

	return p0 +
		( p1 - p0 ) * weights.x +
		( p2 - p0 ) * weights.y * ( 1.f - weights.x ) +
		( p3 - p1 ) * weights.y * weights.x;
}

float PerlinNoise( float2 uv, float scale, float freq, float amplitude, int octaves )
{
	float noise = 0.f;

	float localAmplitude = amplitude;
	float localFrecuency = freq;

	for ( int i = 0; i < octaves; ++i )
	{
		noise += Noise( uv, scale * localFrecuency ) * localAmplitude;

		localAmplitude *= 0.25f;
		localFrecuency *= 3.f;
	}

	return noise * noise;
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint2 dims;
	WeatherTex.GetDimensions( dims.x, dims.y );

	if ( DTid.x < dims.x && DTid.y < dims.y )
	{
		float2 uv = float2( float( DTid.x + 2.f ) / dims.x, float( DTid.y ) / dims.y );
		float2 suv = float2( uv.x + 5.5f, uv.y + 5.5f );

		float perlinAmplitude = 0.5;
		float perlinFrecuency = 0.8;
		float perlinScale = 100.0;
		int perlinOctaves = 4;

		float coverage = saturate( PerlinNoise( uv, perlinScale * 0.95f, perlinFrecuency, perlinAmplitude, perlinOctaves ) );
		float cloudType = saturate( PerlinNoise( suv, perlinScale * 3.0, 0.3f, 0.7f, 10.f ) );
		WeatherTex[DTid.xy] = float4( coverage, cloudType, 0, 1 );
	}
}