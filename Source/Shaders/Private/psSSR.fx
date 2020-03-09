#include "psCommon.fxh"
#include "texCommon.fxh"

Texture2D framebufferTex : register( t1 );
Texture2D depthbufferTex : register( t2 );

static const int g_maxBinarySearchStep = 40;
//static const int g_maxRayStep = 70;
static const float g_depthbias = 0.0001f;
//static const float g_rayStepScale = 1.05f;
//static const float g_maxThickness = 1.8f / g_FarPlaneDist;
//static const float g_maxRayLength = 20.f;

cbuffer SSRConstantBuffer : register(b3)
{
	matrix g_projectionMatrix;
	float padding;							// g_depthbias
	float g_rayStepScale;
	float g_maxThickness;
	float g_maxRayLength;
	int g_maxRayStep;
}

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 viewNormal : NORMAL;
	float3 viewPos : POSITION;
};

float Noise( float2 seed )
{
	return frac( sin( dot( seed.xy, float2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

float3 GetTexCoordXYLinearDepthZ( float3 viewPos )
{
	float4 projPos = mul( float4(viewPos, 1.f), g_projectionMatrix );
	projPos.xy /= projPos.w;
	projPos.xy = projPos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	projPos.z = viewPos.z / g_FarPlaneDist;

	return projPos.xyz;
}

float4 BinarySearch( float3 dir, float3 viewPos )
{
	float3 texCoord = float3( 0.f, 0.f, 0.f );
	float srcdepth = 0.f;
	float depthDiff = 0.f;

	[loop]
	for ( int i = 0; i < g_maxBinarySearchStep; ++i )
	{
		texCoord = GetTexCoordXYLinearDepthZ( viewPos );
		srcdepth = depthbufferTex.SampleLevel( baseSampler, texCoord.xy, 0 ).x;
		depthDiff = srcdepth.x - texCoord.z;

		if ( depthDiff > 0.f )
		{
			viewPos += dir;
			dir *= 0.5f;
		}

		viewPos -= dir;
	}

	texCoord = GetTexCoordXYLinearDepthZ( viewPos );
	srcdepth = depthbufferTex.SampleLevel( baseSampler, texCoord.xy, 0 ).x;
	depthDiff = abs( srcdepth - texCoord.z );
	float4 result = float4( 0.f, 0.f, 0.f, 0.f );
	if ( texCoord.z < 0.9f && depthDiff < g_depthbias )
	{
		result = framebufferTex.SampleLevel( baseSampler, texCoord.xy, 0 );
	}

	return result;
}

float4 main( PS_INPUT input ) : SV_TARGET
{
	float3 incidentVec = normalize( input.viewPos );
	float3 viewNormal = normalize( input.viewNormal );

	float3 reflectVec = reflect( incidentVec, viewNormal );
	reflectVec = normalize( reflectVec );
	reflectVec *= g_rayStepScale;

	float3 reflectPos = input.viewPos;

	float thickness = g_maxThickness / g_FarPlaneDist;

	[loop]
	for ( int i = 0; i < g_maxRayStep; ++i )
	{
		float3 texCoord = GetTexCoordXYLinearDepthZ( reflectPos );
		float srcdepth = depthbufferTex.SampleLevel( baseSampler, texCoord.xy, 0 ).x;

		float depthDiff = texCoord.z - srcdepth;
		if ( depthDiff > g_depthbias && depthDiff < thickness )
		{
			float4 reflectColor = BinarySearch( reflectVec, reflectPos );

			float edgeFade = 1.f - pow( length( texCoord.xy - 0.5f ) * 2.f, 2.f );
			reflectColor.a *= pow( 0.75f, (length( reflectPos - input.viewPos ) / g_maxRayLength) ) * edgeFade;
			return reflectColor;
		}
		else
		{
			reflectPos += ( i + Noise( texCoord.xy ) ) * reflectVec;
		}
	}

	return float4(0.f, 0.f, 0.f, 0.f);
}