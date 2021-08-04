#include "constants.fxh"

cbuffer PS_PASS_CONSTANT : register( b3 )
{
	float		g_nearPlaneDist;
	float		g_FarPlaneDist;
	float		g_elapsedTime;
	float		g_totalTime;
	float2		g_targetSize;
	float2		g_invTargetSize;
};