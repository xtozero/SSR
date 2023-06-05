#include "Common/LightCommon.fxh"

static const float PI = 3.141592657f;

cbuffer VolumetricFogParameter : register( b4 )
{
	float Exposure : packoffset( c0.x );
}

float HenyeyGreensteinPhaseFunction( float3 wi, float3 wo, float g )
{
	float cosTheta = dot( wi, wo );
	float g2 = g * g;
	float denom = pow( 1.f + g2 - 2.f * g * cosTheta, 3.f / 2.f );
	return ( 1.f / ( 4.f * PI ) ) * ( ( 1.f - g2 ) / max( denom, EPSILON ) );
}

float3 ConvertThreadIdToNdc( uint3 id, uint3 dims )
{
	float3 ndc = id;
	ndc *= float3( 2.f / dims.x, -2.f / dims.y, 1.f / dims.z ); // 0 ~ 2
	ndc += float3( -1.f, 1.f, 0.f ); // -1 ~ 1
	return ndc;
}

float ConvertNdcZToDepth( float ndcZ )
{
	return pow( ndcZ, 2.f ) * ( FarPlaneDist - NearPlaneDist ) + NearPlaneDist;
}

float ConvertDepthToNdcZ( float depth )
{
	return pow( ( depth - NearPlaneDist ) / ( FarPlaneDist - NearPlaneDist ), 1 / 2.f );
}

float3 ConvertThreadIdToWorldPosition( uint3 id, uint3 dims )
{
	// id -> ndc
	float3 ndc = ConvertThreadIdToNdc( id, dims );
	float depth = ConvertNdcZToDepth( ndc.z );

	// view ray
	float3 viewRay = mul( float4( ndc.xy, 0.f, 1.f ), InvProjectionMatrix ).xyz;
	viewRay /= viewRay.z;

	// ndc -> world position
	float4 worldPosition = mul( float4( viewRay * depth, 1.f ), InvViewMatrix );

	return worldPosition.xyz;
}

float SliceTickness( float ndcZ, uint dimZ )
{
	return ConvertNdcZToDepth( ndcZ + 1.f / float( dimZ ) ) - ConvertNdcZToDepth( ndcZ );
}