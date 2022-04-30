#include "Common/ViewConstant.fxh"
#include "Common/NormalCompression.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldNormal : NORMAL;
};

struct Output
{
	float depth : SV_Target0;
	float4 packedNormal : SV_Target1;
};

Output main( PS_INPUT input )
{
	Output output = (Output)0;
	output.depth = input.position.w / FarPlaneDist;
	float3 enc = SignedOctEncode( normalize( input.worldNormal ) );
	output.packedNormal = float4( 0.f, enc );
	
	return output;
}