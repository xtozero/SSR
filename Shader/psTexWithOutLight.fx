#include "texCommon.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_Target
{
	return Sample( input.texcoord );
}