#include "texCommon.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_Target
{
	float4 color = Sample( input.texcoord );

	return float4(color.x, color.x, color.x, 1.0f);
}