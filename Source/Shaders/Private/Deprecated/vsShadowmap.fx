#include "Common/VsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 shadowCoord : TEXCOORD0;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix lightWorldView = mul( g_worldMatrix, g_lightViewMatrix );
	float4 lightViewPos = mul( float4(input.position, 1.0f), lightWorldView );
	lightViewPos.z += g_zBias;

	output.position = mul( lightViewPos, g_lightProjectionMatrix );
	output.shadowCoord = output.position.zw;

	return output;
}