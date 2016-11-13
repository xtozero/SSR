#include "vsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 viewNormal : NORMAL;
	float3 viewPos : POSITION;
};


VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix worldView = mul( g_worldMatrix, g_viewMatrix );

	output.viewPos = mul( float4(input.position, 1.f), worldView ).xyz;
	output.position = mul( float4(output.viewPos, 1.f), g_projectionMatrix );

	output.viewNormal = mul( input.normal, (float3x3)transpose( g_invWorldMatrix ) );
	output.viewNormal = mul( output.viewNormal, (float3x3) g_invWorldMatrix );

	return output;
}