cbuffer WORLD : register(b0)
{
	matrix g_worldMatrix : packoffset(c0);
	matrix g_invWorldMatrix : packoffset(c4);
}

cbuffer LIGHT_VEIW_PROJECTION : register(b2)
{
	matrix g_lightViewMatrix : packoffset(c0);
	matrix g_lightProjectionMatrix: packoffset(c4);
};

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
	float4 shadowCoord : TEXCOORD1;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix lightWorldViewPorjection = mul( g_worldMatrix, g_lightViewMatrix );
	lightWorldViewPorjection = mul( lightWorldViewPorjection, g_lightProjectionMatrix );

	output.position = mul( float4(input.position, 1.0f), lightWorldViewPorjection );
	output.shadowCoord = output.position;

	return output;
}