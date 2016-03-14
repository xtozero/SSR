cbuffer WORLD : register(b0)
{
	matrix g_worldMatrix : packoffset(c0);
}

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = mul( float4(input.position, 1.0f), g_worldMatrix );
	output.texcoord = input.texcoord;
	output.color = input.color;

	return output;
}