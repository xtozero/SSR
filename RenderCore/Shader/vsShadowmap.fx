#include "vsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 shadowCoord : TEXCOORD1;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix lightWorldViewPorjection = mul( g_worldMatrix, g_lightViewProjectionMatrix );

	output.position = mul( float4(input.position, 1.0f), lightWorldViewPorjection );
	output.shadowCoord = output.position;

	return output;
}