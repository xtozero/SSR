#include "Common/LightCommon.fxh"
#include "Common/NormalCompression.fxh"

PS_OUTPUT main( PS_INPUT input )
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.frame = CalcLight( input, Sample( input.texcoord ) );
	output.normal.x = PackedSpheremapEncode( normalize( input.normal ) );
	output.depth.x = input.viewPos.z / FarPlaneDist;
	return output;
}