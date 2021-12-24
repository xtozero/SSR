#include "ViewConstant.fxh"
#include "NormalCompression.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldNormal : NORMAL;
};

struct Output
{
	float depth : SV_Target0;
	float2 packedNormal : SV_Target1;
};

Output main( PS_INPUT input ) : SV_Target
{
	Output output = (Output)0;
	output.depth = input.position.w / g_FarPlaneDist;
	output.packedNormal = SpheremapEncode( normalize( input.worldNormal ) );
	
	return output;
}