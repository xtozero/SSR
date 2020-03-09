#include "psCommon.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
    float3 viewPosition : POSITION;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
    return input.viewPosition.z / g_FarPlaneDist;
}