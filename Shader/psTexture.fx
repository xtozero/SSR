#include "lightCommon.fxh"

float4 main( PS_INPUT input ) : SV_Target
{
	float4 color = Sample( input.texcoord );

	return CalcLight( input, color );
}