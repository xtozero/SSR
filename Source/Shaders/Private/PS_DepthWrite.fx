#include "Common/ViewConstant.fxh"
#include "Common/NormalCompression.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 curFramePosition : POSITION0;
	float4 prevFramePosition : POSITION1;
	float3 worldNormal : NORMAL;
};

struct Output
{
	float depth : SV_Target0;
	float4 packedNormal : SV_Target1;
	float2 velocity : SV_Target2;
};

float2 CalcVelocity( float4 curFramePosition, float4 prevFramePosition )
{
	float2 curFrameUV = curFramePosition.xy / curFramePosition.w;
	curFrameUV = curFrameUV * 0.5f + 0.5f;
	curFrameUV.y = 1.f - curFrameUV.y;

	float2 prevFrameUV = prevFramePosition.xy / prevFramePosition.w;
	prevFrameUV = prevFrameUV * 0.5f + 0.5f;
	prevFrameUV.y = 1.f - prevFrameUV.y;

	return curFrameUV - prevFrameUV;
}

Output main( PS_INPUT input )
{
	Output output = (Output)0;
	output.depth = input.position.w / FarPlaneDist;
	float3 enc = SignedOctEncode( normalize( input.worldNormal ) );
	output.packedNormal = float4( 0.f, enc );
	output.velocity = CalcVelocity( input.curFramePosition, input.prevFramePosition );
	
	return output;
}