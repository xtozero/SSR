#include "ViewConstant.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
};

float4 main( PS_INPUT input ) : SV_Target
{
	float depth = input.position.w / g_FarPlaneDist;
	return depth;
}