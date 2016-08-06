#include "lightCommon.fxh"

PS_OUTPUT main( PS_INPUT input )
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.frame = CalcLight( input, float4( input.color, 1.0f ) );
	output.normal.xyz = normalize( input.normal );
	return output;
}