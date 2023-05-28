#include "Common/LightCommon.fxh"

static const float PI = 3.141592657f;

float HenyeyGreensteinPhaseFunction( float3 wi, float3 wo, float g )
{
	float cosTheta = dot( wi, wo );
	float g2 = g * g;
	float denom = pow( 1.f + g2 + 2.f * g * cosTheta, 3.f / 2.f );
	return ( 1.f / ( 4.f * PI ) ) * ( ( 1.f - g2 ) / max( denom, EPSILON ) );
}

float3 ConvertThreadIdToWorldPosition( uint3 id, uint3 dims )
{
	// id -> ndc
	float3 ndc = id;
	ndc = ( ndc + 0.5f ) / float3( dims ); // 0 ~ 1
	ndc.xy = ndc.xy * 2.f - 1.f; // -1 ~ 1
	ndc.y = -ndc.y;

	// ndc -> world position
	float4 worldPosition = mul( float4( ndc, 1.f ), InvViewProjectionMatrix );
	worldPosition /= worldPosition.w;

	return worldPosition.xyz;
}