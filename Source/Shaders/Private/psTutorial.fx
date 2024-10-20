#include "lightCommon.fxh"
#include "normalCompression.fxh"

PS_OUTPUT main( PS_INPUT input )
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.frame = CalcLight( input, float4( input.color, 1.0f ) );
	output.normal.x = PackedSpheremapEncode( normalize( input.normal ) );
	output.depth.x = input.viewPos.z / g_FarPlaneDist;
	return output;
}