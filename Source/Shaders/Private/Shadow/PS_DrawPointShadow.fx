#include "Common/LightCommon.fxh"
#include "Common/NormalCompression.fxh"
#include "Common/ViewConstant.fxh"
#include "Shadow/ShadowConstant.fxh"

Texture2D ViewSpaceDistance : register( t1 );
SamplerState ViewSpaceDistanceSampler : register( s1 );

TextureCube ShadowTexture : register( t2 );
SamplerComparisonState ShadowSampler : register( s2 );

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION0;
	float3 viewRay : POSITION1;
	float3 worldRay : POSITION2;
	float2 uv : TEXCOORD0;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	float viewSpaceDistance = ViewSpaceDistance.Sample( ViewSpaceDistanceSampler, input.uv ).x;
	float3 viewPosition = normalize( input.viewRay ) * viewSpaceDistance;

	float4 worldPosition = mul( float4( viewPosition, 1.f ), InvViewMatrix );
	worldPosition /= worldPosition.w;

    float3 fromLight = worldPosition.xyz - LightPosOrDir.xyz;

    float currentDepth = ( length( fromLight ) - ConstantBias ) / GetLight( LightIdx ).m_range;
    float visibility = ShadowTexture.SampleCmpLevelZero( ShadowSampler, normalize( fromLight ), currentDepth ).r;

	return lerp( 0.5f, 1.f, visibility );
}