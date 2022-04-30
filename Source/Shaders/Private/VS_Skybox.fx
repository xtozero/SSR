#include "Common/ViewConstant.fxh"

struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 ori_position : POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float3 projPos = mul( input.position, (float3x3)ViewMatrix );
	output.position = mul( float4( projPos, 1.0f ), ProjectionMatrix );

	output.ori_position = input.position;

	return output;
}