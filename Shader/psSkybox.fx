SamplerState baseSampler : register( s0 );
TextureCube baseTexture : register( t0 );

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 ori_position : POSITION;
};

float4 main( PS_INPUT input ) : SV_Target
{
	return baseTexture.Sample( baseSampler, normalize( input.ori_position ) );
}