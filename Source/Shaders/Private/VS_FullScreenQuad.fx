#include "ViewConstant.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION0;
	float3 viewRay : POSITION1;
	float2 uv : TEXCOORD0;
};

VS_OUTPUT main( uint vertexId : SV_VertexID )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.uv = float2( ( vertexId << 1 ) & 2, vertexId & 2 );
	output.position = float4( output.uv * float2( 2, -2 ) + float2( -1, 1 ), 0, 1 );

	float4 worldPosition = mul( float4( output.position.xy, 0, 1 ), g_invViewProjectionMatrix );
	output.worldPosition = worldPosition.xyz / worldPosition.w;

	float4 viewRay = mul( float4( output.position.xy, 1, 1 ), g_invProjectionMatrix );
	viewRay /= viewRay.w;
	output.viewRay = viewRay.xyz;

	return output;
}