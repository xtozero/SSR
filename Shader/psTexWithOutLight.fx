#include "lightCommon.fxh"

float4 main( PS_INPUT input ) : SV_Target
{
	return Sample( input.texcoord );
}