#include "Common/VsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 viewPos : POSITION1;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.worldPos = mul( float4(input.position, 1.0f), g_worldMatrix ).xyz;
	output.viewPos = mul( float4(output.worldPos, 1.0f), g_viewMatrix ).xyz;
	output.position = mul( float4(output.viewPos, 1.0f), g_projectionMatrix );
	output.normal = mul( float4(input.normal, 0.f), transpose( g_invWorldMatrix ) ).xyz;
	output.color = float4( 1.0f, 0.0f, 1.0f, 1.0f ); // dummy color
	output.texcoord = input.texcoord;

	return output;
}