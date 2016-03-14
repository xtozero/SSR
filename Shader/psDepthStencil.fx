SamplerState baseSampler : register(s0);
Texture2D baseTexture : register(t0);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_Target
{
	float4 color = baseTexture.Sample( baseSampler, input.texcoord );

	return float4(color.x, color.x, color.x, 1.0f);
}