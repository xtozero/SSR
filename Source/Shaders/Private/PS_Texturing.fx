#include "Common/LightCommon.fxh"

Texture2D DiffuseTex : register( t2 );
SamplerState DiffuseTexSampler : register( s2 );

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPos : POSITION0;
    float3 viewPos : POSITION1;
    float4 projectionPos : POSITION2;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_Target0
{
    GeometryProperty geometry = (GeometryProperty)0;
    geometry.worldPos = input.worldPos;
    geometry.viewPos = input.viewPos;
    geometry.normal = input.normal;
    geometry.screenUV = ( input.projectionPos.xy / input.projectionPos.w ) * float2( 0.5f, -0.5f ) + 0.5f;

    LIGHTCOLOR cColor = CalcLight( geometry );

	float4 lightColor = cColor.m_diffuse * MoveLinearSpace( DiffuseTex.Sample( DiffuseTexSampler, input.texcoord ) );
	lightColor += cColor.m_specular * MoveLinearSpace( Specular );

	return float4( lightColor.rgb, 1.f );
}