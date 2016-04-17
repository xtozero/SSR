#include "lightCommon.fxh"

float4 main( PS_INPUT input ) : SV_Target
{
	return CalcLight( input, float4( input.color, 1.0f ) );
}