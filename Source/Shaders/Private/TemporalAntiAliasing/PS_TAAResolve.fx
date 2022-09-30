Texture2D HistoryTex : register( t0 );
SamplerState HistoryTexSampler : register( s0 );
Texture2D SceneTex : register( t1 );
SamplerState SceneTexSampler : register( s1 );
Texture2D VelocityTex : register( t2 );
SamplerState VelocityTexSampler : register( s2 );

cbuffer TAAParameter : register( b1 )
{
	float BlendWeight : packoffset( c0 );
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION0;
	float3 viewRay : POSITION1;
	float3 worldRay : POSITION2;
	float2 uv : TEXCOORD0;
};

float4 main( PS_INPUT input ) : SV_Target0
{
	float2 velocity = VelocityTex.Sample( VelocityTexSampler, input.uv );
	float2 previousUV = input.uv - velocity;

	float3 historyColor = HistoryTex.Sample( HistoryTexSampler, previousUV ).rgb;
	float4 sceneColor = SceneTex.Sample( SceneTexSampler, input.uv );

	float3 left = SceneTex.Sample( SceneTexSampler, input.uv, int2( -1, 0 ) ).rgb;
	float3 right = SceneTex.Sample( SceneTexSampler, input.uv, int2( 1, 0 ) ).rgb;
	float3 top = SceneTex.Sample( SceneTexSampler, input.uv, int2( 0, -1 ) ).rgb;
	float3 bottom = SceneTex.Sample( SceneTexSampler, input.uv, int2( 0, 1 ) ).rgb;

	float3 lower = min( sceneColor, min( min( left, right ), min( top, bottom ) ) );
	float3 upper = max( sceneColor, max( max( left, right ), max( top, bottom ) ) );

	historyColor = clamp( historyColor, lower, upper );
	float3 resolveColor = lerp( sceneColor.rgb, historyColor, BlendWeight );

	return float4( resolveColor, sceneColor.a );
}