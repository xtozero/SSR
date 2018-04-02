cbuffer WORLD : register(b0)
{
	matrix g_worldMatrix : packoffset(c0);
	matrix g_invWorldMatrix : packoffset(c4);
}

cbuffer VEIW_PROJECTION : register(b1)
{
	matrix g_viewMatrix : packoffset(c0);
	matrix g_projectionMatrix : packoffset(c4);
};

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
	float3 worldPos : POSITION0;
	float3 viewPos : POSITION1;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
	float4 shadowCoord : TEXCOORD1;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.worldPos = mul( float4(input.position, 1.0f), g_worldMatrix ).xyz;
	output.viewPos = mul( float4(output.worldPos, 1.0f), g_viewMatrix ).xyz;
	output.position = mul( float4(output.viewPos, 1.0f), g_projectionMatrix );
	output.normal = mul( float4(input.normal, 0.f), transpose( g_invWorldMatrix ) ).xyz;
	output.texcoord = input.texcoord;

	matrix lightWorldViewPorjection = mul( g_worldMatrix, g_lightViewMatrix );
	lightWorldViewPorjection = mul( lightWorldViewPorjection, g_lightProjectionMatrix );

	output.shadowCoord = mul( float4(input.position, 1.0f), lightWorldViewPorjection );
	output.color = input.color;

	return output;
}