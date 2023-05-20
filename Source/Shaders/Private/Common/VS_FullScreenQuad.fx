#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION0;
	float3 viewRay : POSITION1;
	float3 worldRay : POSITION2;
	float2 uv : TEXCOORD0;
};

VS_OUTPUT main( uint vertexId : SV_VertexID )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.uv = float2( ( vertexId << 1 ) & 2, vertexId & 2 );
	output.position = float4( output.uv * float2( 2, -2 ) + float2( -1, 1 ), 0.f, 1.f );

	output.uv = ApplyTAAJittering( output.position ).xy * float2( 0.5, -0.5 ) + float2( 0.5, 0.5f );

	float4 worldPosition = mul( float4( output.position.xy, 0.f, 1.f ), InvViewProjectionMatrix );
	output.worldPosition = worldPosition.xyz / worldPosition.w;

	float4 viewRay = mul( float4( output.position.xy, 1.f, 1.f ), InvProjectionMatrix );
	viewRay /= viewRay.w;
	output.viewRay = viewRay.xyz;

	float4 worldRay = mul( float4( output.position.xy, 1.f, 1.f ), InvViewProjectionMatrix );
	worldRay /= worldRay.w;
	output.worldRay = worldRay.xyz;

	return output;
}