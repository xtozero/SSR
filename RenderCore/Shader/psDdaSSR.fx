#include "psCommon.fxh"
#include "texCommon.fxh"

Texture2D framebufferTex : register( t1 );
Texture2D depthbufferTex : register( t2 );
Texture2D backfaceDepthBufferTex : register( t3 );

cbuffer SSRConstantBuffer : register(b3)
{
	matrix g_projectionMatrix;
	float g_depthbias;
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

void swap( inout float lhs, inout float rhs )
{
	float temp = lhs;
	lhs = rhs;
	rhs = temp;
}

float Noise( float2 seed )
{
	return frac( sin( dot( seed.xy, float2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

bool TraceScreenSpaceRay( float3 dir, float3 viewPos, out float3 hitPixel_alpha  )
{
	float rayLength = ( viewPos.z * dir.z * g_maxRayLength ) < g_nearPlaneDist ? ( g_nearPlaneDist - viewPos.z ) / dir.z : g_maxRayLength;

	hitPixel_alpha = float3( -1, -1, 1 );

	float3 rayEnd = viewPos + dir * rayLength;

	float4 ssRayBegin = mul( float4( viewPos, 1.0 ), g_projectionMatrix );
	float4 ssRayEnd = mul( float4( rayEnd, 1.0 ), g_projectionMatrix );

	float k0 = 1 / ssRayBegin.w;
	float k1 = 1 / ssRayEnd.w;

	float Q0 = viewPos.z * k0;
	float Q1 = rayEnd.z * k1;

	float2 P0 = ssRayBegin.xy * k0;
	float2 P1 = ssRayEnd.xy * k1;

	P0 = ( P0 * 0.5 + 0.5 ) * g_targetSize;
	P1 = ( P1 * 0.5 + 0.5 ) * g_targetSize;

	P1 += ( dot( P1 - P0, P1 - P0 ) < 0.0001 ) ? float2( 0.01, 0.01 ) : float2( 0, 0 );
	float2 delta = P1 - P0;

	bool permute = false;
	if ( abs( delta.x ) < abs( delta.y ) )
	{
		permute = true;
		delta = delta.yx;
		P0 = P0.yx;
		P1 = P1.yx;
	}

	float stepDir = sign( delta.x );
	float invdx = stepDir / delta.x;

	float stepCount = 0;
	float rayZ = viewPos.z;
	float sceneZMax = viewPos.z + 100.f;

	float end = stepDir * P1.x;

	float4 PQk = float4( P0, Q0, k0 );
	float4 dPQk = float4( float2( stepDir, delta.y * invdx ), ( Q1 - Q0 ) * invdx, ( k1 - k0 ) * invdx );

	dPQk *= g_rayStepScale;

	// float jitter = Noise( PQk.xy );
	// PQk += dPQk * jitter;

	float thickness = g_maxThickness;

	[loop]
	for ( ;
		( ( PQk.x * stepDir ) <= end ) &&
		( stepCount < g_maxRayStep ) &&
		( ( rayZ < sceneZMax ) || 
		( ( rayZ - thickness ) > sceneZMax ) ) &&
		( sceneZMax != 0.0 );
		PQk += dPQk,
		stepCount += 1 )
	{
		hitPixel_alpha.xy = permute ? PQk.yx : PQk.xy;
		hitPixel_alpha.xy *= g_invTargetSize;
		hitPixel_alpha.y = 1 - hitPixel_alpha.y;

		rayZ = PQk.z / PQk.w;

		sceneZMax = depthbufferTex.SampleLevel( baseSampler, hitPixel_alpha.xy, 0 ).x;
		thickness = backfaceDepthBufferTex.SampleLevel( baseSampler, hitPixel_alpha.xy, 0 ).x * g_FarPlaneDist;
		sceneZMax *= g_FarPlaneDist;
		thickness -= sceneZMax;
		sceneZMax += g_depthbias;
	}

	float edgeFade = 1.f - pow( length( hitPixel_alpha.xy - 0.5 ) * 2.f, 3.0f );
	hitPixel_alpha.z = edgeFade;
	return ( rayZ >= sceneZMax ) && ( rayZ - thickness <= sceneZMax );
}

float4 main( PS_INPUT input ) : SV_TARGET
{
	float3 incidentVec = normalize( input.viewPos );
	float3 viewNormal = normalize( input.viewNormal );

	float3 reflectVec = reflect( incidentVec, viewNormal );
	reflectVec = normalize( reflectVec );

	float3 hitPixel_alpha;
	bool isHit = TraceScreenSpaceRay( reflectVec, input.viewPos, hitPixel_alpha );

	float3 color = framebufferTex.SampleLevel( baseSampler, hitPixel_alpha.xy, 0 ).rgb;

	return float4( color, hitPixel_alpha.z ) * isHit;
}