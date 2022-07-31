#include "Common/VsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = mul( float4(input.position, 1.0f), g_worldMatrix );
	output.texcoord = input.texcoord;

	return output;
}