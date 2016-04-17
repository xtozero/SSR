SamplerState baseSampler : register( s0 );
Texture2D baseTexture : register( t0 );

float4 Sample( float2 texcoord )
{
	return baseTexture.Sample( baseSampler, texcoord );
}