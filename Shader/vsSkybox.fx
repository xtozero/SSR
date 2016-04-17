cbuffer WORLD : register( b0 )
{
	matrix g_worldMatrix : packoffset( c0 );
	matrix g_invWorldMatrix : packoffset(c4);
}

cbuffer VEIW_PROJECTION : register( b1 )
{
	matrix g_viewMatrix : packoffset( c0 );
	matrix g_projectionMatrix : packoffset( c4 );
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
	float3 ori_position : POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix worldViewPorjection = mul( g_worldMatrix, g_viewMatrix );
	worldViewPorjection = mul( worldViewPorjection, g_projectionMatrix );

	output.position = mul( float4( input.position, 1.0f ), worldViewPorjection );

	output.ori_position = input.position;

	return output;
}