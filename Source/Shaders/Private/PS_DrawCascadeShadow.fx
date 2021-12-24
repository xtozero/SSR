#include "NormalCompression.fxh"
#include "ShadowCommon.fxh"
#include "ViewConstant.fxh"

Texture2D SceneDepth : register( t1 );
SamplerState SceneDepthSampler : register( s1 );

Texture2D WorldNormal : register( t3 );
SamplerState WorldNormalSampler : register( s3 );

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	float x = lerp( -1, 1, input.uv.x );
	float y = lerp( 1, -1, input.uv.y );

	float4 viewPosition = mul( float4( x, y, 1, 1 ), g_invProjectionMatrix );
	viewPosition /= viewPosition.w;
	
	float depth = SceneDepth.Sample( SceneDepthSampler, input.uv );
	viewPosition.xyz *= depth;
	
	float4 worldPosition = mul( viewPosition, g_invViewMatrix );
	worldPosition /= worldPosition.w;
	
	float2 packedNormal = WorldNormal.Sample( WorldNormalSampler, input.uv ).xy;
	float3 worldNormal = SpheremapDecode( packedNormal );
	
	float visibility = CalcShadowVisibility( worldPosition, worldNormal, viewPosition );
	
	return visibility;
}