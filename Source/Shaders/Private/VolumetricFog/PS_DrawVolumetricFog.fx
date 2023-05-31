#include "VolumetricFog/VolumetricFogCommon.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION0;
	float3 viewRay : POSITION1;
	float3 worldRay : POSITION2;
	float2 uv : TEXCOORD0;
};

Texture3D ScatteringTex : register( t0 );
SamplerState ScatteringTexSampler : register( s0 );

Texture2D ViewSpaceDistance : register( t1 );
SamplerState ViewSpaceDistanceSampler : register( s1 );

float4 main( PS_INPUT input ) : SV_Target0
{
	float viewSpaceDistance = ViewSpaceDistance.Sample( ViewSpaceDistanceSampler, input.uv ).x;
	if ( viewSpaceDistance <= 0.f )
	{
		viewSpaceDistance = FarPlaneDist;
	}

	float3 viewPosition = normalize( input.viewRay ) * viewSpaceDistance;

	float4 ndc = mul( float4( viewPosition, 1.f ), ProjectionMatrix );
	ndc /= ndc.w;

	float3 uv = ndc.xyz;
	uv.xy = uv.xy * 0.5f + 0.5f;
	uv.y = 1.f - uv.y;

	float4 scatteringColorAndTransmittance = ScatteringTex.Sample( ScatteringTexSampler, uv );

	float3 scatteringColor = scatteringColorAndTransmittance.rgb;
	scatteringColor /= scatteringColor + 1.f;

	return float4( scatteringColor, scatteringColorAndTransmittance.a );
}