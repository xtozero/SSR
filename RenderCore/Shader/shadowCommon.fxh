#include "texCommon.fxh"

Texture2D shadowTexture : register( t2 );
SamplerState shadowSampler : register( s1 );

cbuffer SHADOW_CONSTANT : register( b3 )
{
	float g_bias;
	float3 padding;
};

float SampleShadow( float2 texcoord )
{
	return shadowTexture.Sample( shadowSampler, texcoord ).r;
}

float PoissonSample4x4Shadow( float2 texcoord, int i, int j )
{
	float2 offset = PoissonSample4x4( i, j ) * 0.00048828125f;

	return shadowTexture.Sample( shadowSampler, texcoord + offset ).r;
}

float RotatePoissonSample4x4Shadow( float2 texcoord, float2 sin_cos, int i, int j )
{
	float2 offset = RotatePoissonSample4x4( sin_cos.x, sin_cos.y, i, j ) * 0.00048828125f;
	
	return shadowTexture.Sample( shadowSampler, texcoord + offset ).r;
}