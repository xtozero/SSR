#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float3 viewPos = mul( float4( input.position, 1.0f ), ViewMatrix ).xyz;
	float4 projectionPos = mul( float4( viewPos, 1.0f ), ProjectionMatrix );
	output.position = ApplyTAAJittering( projectionPos );
	
	return output;
}