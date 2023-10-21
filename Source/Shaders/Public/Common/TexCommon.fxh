SamplerState baseSampler : register( s0 );
Texture2D baseTexture : register( t0 );

float4 Sample( float2 texcoord )
{
	return baseTexture.Sample( baseSampler, texcoord );
}

float random( float3 seed )
{
	return frac( sin( dot( seed, float3( 53.1215, 21.1352, 9.1322 ) ) ) * 43758.5453 );
}

static const float2 PoissonSamples[64] =
{
	float2( -0.5119625f, -0.4827938f ),
	float2( -0.2171264f, -0.4768726f ),
	float2( -0.7552931f, -0.2426507f ),
	float2( -0.7136765f, -0.4496614f ),
	float2( -0.5938849f, -0.6895654f ),
	float2( -0.3148003f, -0.7047654f ),
	float2( -0.42215f, -0.2024607f ),
	float2( -0.9466816f, -0.2014508f ),
	float2( -0.8409063f, -0.03465778f ),
	float2( -0.6517572f, -0.07476326f ),
	float2( -0.1041822f, -0.02521214f ),
	float2( -0.3042712f, -0.02195431f ),
	float2( -0.5082307f, 0.1079806f ),
	float2( -0.08429877f, -0.2316298f ),
	float2( -0.9879128f, 0.1113683f ),
	float2( -0.3859636f, 0.3363545f ),
	float2( -0.1925334f, 0.1787288f ),
	float2( 0.003256182f, 0.138135f ),
	float2( -0.8706837f, 0.3010679f ),
	float2( -0.6982038f, 0.1904326f ),
	float2( 0.1975043f, 0.2221317f ),
	float2( 0.1507788f, 0.4204168f ),
	float2( 0.3514056f, 0.09865579f ),
	float2( 0.1558783f, -0.08460935f ),
	float2( -0.0684978f, 0.4461993f ),
	float2( 0.3780522f, 0.3478679f ),
	float2( 0.3956799f, -0.1469177f ),
	float2( 0.5838975f, 0.1054943f ),
	float2( 0.6155105f, 0.3245716f ),
	float2( 0.3928624f, -0.4417621f ),
	float2( 0.1749884f, -0.4202175f ),
	float2( 0.6813727f, -0.2424808f ),
	float2( -0.6707711f, 0.4912741f ),
	float2( 0.0005130528f, -0.8058334f ),
	float2( 0.02703013f, -0.6010728f ),
	float2( -0.1658188f, -0.9695674f ),
	float2( 0.4060591f, -0.7100726f ),
	float2( 0.7713396f, -0.4713659f ),
	float2( 0.573212f, -0.51544f ),
	float2( -0.3448896f, -0.9046497f ),
	float2( 0.1268544f, -0.9874692f ),
	float2( 0.7418533f, -0.6667366f ),
	float2( 0.3492522f, 0.5924662f ),
	float2( 0.5679897f, 0.5343465f ),
	float2( 0.5663417f, 0.7708698f ),
	float2( 0.7375497f, 0.6691415f ),
	float2( 0.2271994f, -0.6163502f ),
	float2( 0.2312844f, 0.8725659f ),
	float2( 0.4216993f, 0.9002838f ),
	float2( 0.4262091f, -0.9013284f ),
	float2( 0.2001408f, -0.808381f ),
	float2( 0.149394f, 0.6650763f ),
	float2( -0.09640376f, 0.9843736f ),
	float2( 0.7682328f, -0.07273844f ),
	float2( 0.04146584f, 0.8313184f ),
	float2( 0.9705266f, -0.1143304f ),
	float2( 0.9670017f, 0.1293385f ),
	float2( 0.9015037f, -0.3306949f ),
	float2( -0.5085648f, 0.7534177f ),
	float2( 0.9055501f, 0.3758393f ),
	float2( 0.7599946f, 0.1809109f ),
	float2( -0.2483695f, 0.7942952f ),
	float2( -0.4241052f, 0.5581087f ),
	float2( -0.1020106f, 0.6724468f ),
};

float2 PoissonDiskSample( uint i )
{
	return PoissonSamples[i % 64];
}

float2 RotatePoissonDiskSample( float sin, float cos, uint i )
{
	float2 offset = PoissonDiskSample( i );
	offset = float2( offset.x * cos - offset.y * sin, offset.x * sin + offset.y * cos );

	return offset;
}

// https://gist.github.com/Fewes/59d2c831672040452aa77da6eaab2234
float4 Tex3DTricubic( Texture3D tex, SamplerState texSampler, float3 coord )
{
	uint3 dims;
	tex.GetDimensions( dims.x, dims.y, dims.z );

	float3 textureSize = dims;

	// Shift the coordinate from [0,1] to [-0.5, textureSize-0.5]
	float3 coord_grid = coord * textureSize - 0.5f;
	float3 index = floor( coord_grid );
	float3 fraction = coord_grid - index;
	float3 one_frac = 1.0f - fraction;

	float3 w0 = 1.0f / 6.0f * one_frac * one_frac * one_frac;
	float3 w1 = 2.0f / 3.0f - 0.5f * fraction * fraction * ( 2.0f - fraction );
	float3 w2 = 2.0f / 3.0f - 0.5f * one_frac * one_frac * ( 2.0f - one_frac );
	float3 w3 = 1.0f / 6.0f * fraction * fraction * fraction;

	float3 g0 = w0 + w1;
	float3 g1 = w2 + w3;
	float3 mult = 1.0f / textureSize;
	float3 h0 = mult * ( ( w1 / g0 ) - 0.5f + index ); //h0 = w1/g0 - 1, move from [-0.5, textureSize-0.5] to [0,1]
	float3 h1 = mult * ( ( w3 / g1 ) + 1.5f + index ); //h1 = w3/g1 + 1, move from [-0.5, textureSize-0.5] to [0,1]

	// Fetch the eight linear interpolations
	// Weighting and fetching is interleaved for performance and stability reasons
	float4 tex000 = tex.SampleLevel( texSampler, h0, 0 );
	float4 tex100 = tex.SampleLevel( texSampler, float3( h1.x, h0.y, h0.z ), 0 );
	tex000 = lerp( tex100, tex000, g0.x ); // Weigh along the x-direction

	float4 tex010 = tex.SampleLevel( texSampler, float3( h0.x, h1.y, h0.z ), 0 );
	float4 tex110 = tex.SampleLevel( texSampler, float3( h1.x, h1.y, h0.z ), 0 );
	tex010 = lerp( tex110, tex010, g0.x ); // Weigh along the x-direction
	tex000 = lerp( tex010, tex000, g0.y ); // Weigh along the y-direction

	float4 tex001 = tex.SampleLevel( texSampler, float3( h0.x, h0.y, h1.z ), 0 );
	float4 tex101 = tex.SampleLevel( texSampler, float3( h1.x, h0.y, h1.z ), 0 );
	tex001 = lerp( tex101, tex001, g0.x ); // Weigh along the x-direction

	float4 tex011 = tex.SampleLevel( texSampler, float3( h0.x, h1.y, h1.z ), 0 );
	float4 tex111 = tex.SampleLevel( texSampler, h1, 0 );
	tex011 = lerp( tex111, tex011, g0.x ); // Weigh along the x-direction
	tex001 = lerp( tex011, tex001, g0.y ); // Weigh along the y-direction

	return lerp( tex001, tex000, g0.z ); // Weigh along the z-direction
}

// https://www.gamedev.net/forums/topic/687535-implementing-a-cube-map-lookup-function/5337472/
float3 CubeUvToArrayUv( float3 uv )
{
	float3 absUV = abs(uv);
	float3 ret = 0;

	if ( absUV.x >= absUV.y && absUV.x >= absUV.z )
	{
		ret.z = uv.x > 0.f ? 0.f : 1.f;
		ret.xy = float2( uv.x > 0.f ? -uv.z : uv.z, -uv.y );
		ret.xy *= 0.5f / absUV.x;
	}
	else if ( absUV.y >= absUV.z )
	{
		ret.z = uv.y > 0.f ? 2.f : 3.f;
		ret.xy = float2( uv.x, uv.y > 0.f ? uv.z : -uv.z );
		ret.xy *= 0.5f / absUV.y;
	}
	else
	{
		ret.z = uv.z > 0.f ? 4.f : 5.f;
		ret.xy = float2( uv.z > 0.f ? uv.x : -uv.x, -uv.y );
		ret.xy *= 0.5f / absUV.z;
	}

	ret.xy += 0.5f;
	return ret;
}