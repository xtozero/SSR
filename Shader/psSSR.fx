#include "psCommon.fxh"
#include "texCommon.fxh"

Texture2D framebufferTex : register( t1 );
Texture2D depthbufferTex : register( t2 );

static const int g_maxBinarySearchStep = 32;
static const int g_maxRayStep = 100;
static const float g_depthbias = 0.1f;
static const float g_rayStepScale = 1.f;
static const float g_maxThickness = 1.5f / g_FarPlaneDist;

cbuffer SSRConstantBuffer : register(b3)
{
	matrix g_projectionMatrix : packoffset(c0);
}

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 viewNormal : NORMAL;
	float3 viewPos : POSITION;
};

float3 GetTexCoordXYLinearDepthZ( float3 viewPos )
{
	float4 projPos = mul( float4(viewPos, 1.f), g_projectionMatrix );
	projPos.xy /= projPos.w;
	projPos.xy = projPos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	projPos.z = projPos.w / g_FarPlaneDist;

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
	if ( texCoord.z < 0.9f && depthDiff < g_depthbias )
	{
		return framebufferTex.SampleLevel( baseSampler, texCoord.xy, 0 );
	}

	return float4(0.f, 0.f, 0.f, 0.f);
}

float4 main( PS_INPUT input ) : SV_TARGET
{
	float3 incidentVec = normalize( input.viewPos );
	float3 viewNormal = normalize( input.viewNormal );

	float3 reflectVec = reflect( incidentVec, viewNormal );
	reflectVec = normalize( reflectVec );
	reflectVec *= g_rayStepScale;

	float4 reflectColor = float4(0.f, 0.f, 0.f, 0.f);
	float3 reflectPos = input.viewPos + reflectVec;

	[loop]
	for ( int i = 0; i < g_maxRayStep; ++i )
	{
		float3 texCoord = GetTexCoordXYLinearDepthZ( reflectPos );
		float srcdepth = depthbufferTex.SampleLevel( baseSampler, texCoord.xy, 0 ).x;

		if ( texCoord.z - srcdepth > 0 && texCoord.z - srcdepth < g_maxThickness )
		{
			reflectColor = BinarySearch( reflectVec, reflectPos );
			break;
		}
		else
		{
			reflectPos += reflectVec;
		}
	}

	return reflectColor;
}