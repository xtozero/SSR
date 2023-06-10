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

	float3 uv = float3( input.uv, ConvertDepthToNdcZ( viewPosition.z ) );
	float4 scatteringColorAndTransmittance = ScatteringTex.Sample( ScatteringTexSampler, uv );

	float3 scatteringColor = HDR( scatteringColorAndTransmittance.rgb );

	return float4( scatteringColor, scatteringColorAndTransmittance.a );
}