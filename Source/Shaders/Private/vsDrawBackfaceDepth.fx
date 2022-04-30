#include "Common/VsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
    float3 viewPosition : POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

    matrix worldView = mul( g_worldMatrix, g_viewMatrix );

	output.viewPosition = mul( float4(input.position, 1.f), worldView ).xyz;
    output.position = mul( float4(output.viewPosition, 1.f), g_projectionMatrix );

	return output;
}