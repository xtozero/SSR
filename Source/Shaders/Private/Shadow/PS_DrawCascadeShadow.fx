#include "Common/NormalCompression.fxh"
#include "Common/ViewConstant.fxh"
#include "Shadow/ShadowCommon.fxh"

Texture2D ViewSpaceDistance : register( t1 );
SamplerState ViewSpaceDistanceSampler : register( s1 );

Texture2D WorldNormal : register( t3 );
SamplerState WorldNormalSampler : register( s3 );

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

	float4 worldPosition = mul( float4( viewPosition, 1 ), InvViewMatrix );
	worldPosition /= worldPosition.w;

	float3 packedNormal = WorldNormal.Sample( WorldNormalSampler, input.uv ).yzw;
	float3 worldNormal = SignedOctDecode( packedNormal );

	float visibility = CalcShadowVisibilityWithNormalOffset( worldPosition.xyz, worldNormal, viewPosition );

	return lerp( 0.5f, 1, visibility );
}