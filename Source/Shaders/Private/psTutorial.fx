#include "lightCommon.fxh"
// #include "normalCompression.fxh"

float4 main( PS_INPUT input ) : SV_Target0
{
	//PS_OUTPUT output = (PS_OUTPUT)0;

	//output.frame = CalcLight( input, float4( input.color, 1.0f ) );
	//output.normal.x = PackedSpheremapEncode( normalize( input.normal ) );
	//output.depth.x = input.viewPos.z / g_FarPlaneDist;
	//return output;
	
	return CalcLight( input );
}