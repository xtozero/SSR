#include "../vsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION0;
};

VS_OUTPUT main( uint vertexID : SV_VertexID )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	
	float2 uv = float2( ( vertexID & 1 ) << 1, vertexID & 2 );
	float4 pos = float4( float2( -1.f, 1.f ) + uv * float2( 2.f, -2.f ), 0.f, 1.f );
	
	output.position = pos;

	float4x4 invVewProjection = mul( g_invProjectionMatrix, g_invViewMatrix );
	float4 worldPos = mul( float4( pos.xy, 1.f, 1.f ), invVewProjection );
	output.worldPos = worldPos.xyz / worldPos.w;

	return output;
}