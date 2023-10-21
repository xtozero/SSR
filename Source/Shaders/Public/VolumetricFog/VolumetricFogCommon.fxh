#include "Common/Constants.fxh"
#include "Common/LightCommon.fxh"

struct VolumetricFogParameter
{
	float Exposure;
	float DepthPackExponent;
	float NearPlaneDist;
	float FarPlaneDist;
};

cbuffer VolumetricFogParameterBuffer : register( b3 )
{
	VolumetricFogParameter VolumetricFogParam : register( c0 );
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
	ndc += 0.5f;
	ndc *= float3( 2.f / dims.x, -2.f / dims.y, 1.f / dims.z );
	ndc += float3( -1.f, 1.f, 0.f );
	return ndc;
}

float3 ConvertThreadIdToNdc( uint3 id, uint3 dims, float3 jitter )
{
	float3 ndc = id;
	ndc += 0.5f;
	ndc += jitter;
	ndc *= float3( 2.f / dims.x, -2.f / dims.y, 1.f / dims.z );
	ndc += float3( -1.f, 1.f, 0.f );
	return ndc;
}

float ConvertNdcZToDepth( float ndcZ )
{
	float depthPackExponent = VolumetricFogParam.DepthPackExponent;
	float nearPlaneDist = VolumetricFogParam.NearPlaneDist;
	float farPlaneDist = VolumetricFogParam.FarPlaneDist;

	return pow( ndcZ, depthPackExponent ) * ( farPlaneDist - nearPlaneDist ) + nearPlaneDist;
}

float ConvertDepthToNdcZ( float depth )
{
	float depthPackExponent = VolumetricFogParam.DepthPackExponent;
	float nearPlaneDist = VolumetricFogParam.NearPlaneDist;
	float farPlaneDist = VolumetricFogParam.FarPlaneDist;

	return pow( saturate( ( depth - nearPlaneDist ) / ( farPlaneDist - nearPlaneDist ) ), 1 / depthPackExponent );
}

float3 ConvertToWorldPosition( float3 ndc, float depth )
{
	// view ray
	float4 viewRay = mul( float4( ndc, 1.f ), InvProjectionMatrix );
	viewRay /= viewRay.w;
	viewRay /= viewRay.z;

	// ndc -> world position
	float4 worldPosition = mul( float4( viewRay.xyz * depth, 1.f ), InvViewMatrix );

	return worldPosition.xyz;
}

float3 ConvertThreadIdToWorldPosition( uint3 id, uint3 dims )
{
	// id -> ndc
	float3 ndc = ConvertThreadIdToNdc( id, dims );
	float depth = ConvertNdcZToDepth( ndc.z );

	return ConvertToWorldPosition( ndc, depth );
}

float3 ConvertThreadIdToWorldPosition( uint3 id, uint3 dims, float3 jitter )
{
	// id -> ndc
	float3 ndc = ConvertThreadIdToNdc( id, dims, jitter );
	float depth = ConvertNdcZToDepth( ndc.z );

	return ConvertToWorldPosition( ndc, depth );
}

float3 ConvertWorldPositionToUV( float3 worldPosition, matrix viewProjectionMatrix )
{
	float4 pos = mul( float4( worldPosition, 1.f ), viewProjectionMatrix );

	float3 uv = float3( pos.xy / pos.w, ConvertDepthToNdcZ( pos.w ) );
	uv.xy *= float2( 0.5f, -0.5f );
	uv.xy += 0.5f;

	return uv;
}

float SliceTickness( float ndcZ, uint dimZ )
{
	return ConvertNdcZToDepth( ndcZ + 1.f / float( dimZ ) ) - ConvertNdcZToDepth( ndcZ );
}

float3 HDR( float3 l )
{
	l = l * VolumetricFogParam.Exposure;
	l.r = l.r < 1.413f ? pow( abs( l.r * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.r );
	l.g = l.g < 1.413f ? pow( abs( l.g * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.g );
	l.b = l.b < 1.413f ? pow( abs( l.b * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.b );
	return l;
}
