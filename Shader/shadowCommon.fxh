#include "texCommon.fxh"

Texture2D shadowTexture : register( t2 );

float2 poissonDisk[4] = {
	float2( -0.94201624f, -0.39906216f ),
	float2( 0.94558609f, -0.76890725f ),
	float2( -0.094184101f, -0.92938870f ),
	float2( 0.34495938f, 0.29387760f )
};

float SampleShadow( float2 texcoord )
{
	return shadowTexture.Sample( baseSampler, texcoord ).r;
}

float PoissonSampleShadow( float2 texcoord, int i )
{
	return shadowTexture.Sample( baseSampler, texcoord + poissonDisk[i] / 700.f ).r;
}