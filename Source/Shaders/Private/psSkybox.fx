SamplerState SkyTextureSampler : register( s0 );
TextureCube SkyTexture : register( t0 );

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 ori_position : POSITION;
};

float4 main( PS_INPUT input ) : SV_Target
{
	return SkyTexture.Sample( SkyTextureSampler, normalize( input.ori_position ) );
}