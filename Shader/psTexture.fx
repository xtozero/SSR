#include "lightCommon.fxh"

PS_OUTPUT main( PS_INPUT input )
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.frame = CalcLight( input, Sample( input.texcoord ) );
	output.normal.xyz = normalize( input.normal );
	return output;
}