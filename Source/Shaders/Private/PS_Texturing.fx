#include "Common/BindlessResources.fxh"
#include "Common/LightCommon.fxh"

#if SupportsBindless == 1
DefineBindlessIndices
{
    int DiffuseTex;
    int DiffuseTexSampler;
};
#else
Texture2D DiffuseTex : register( t2 );
SamplerState DiffuseTexSampler : register( s2 );
#endif

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

#if SupportsBindless == 1
    float4 lightColor = (float4)0.f;
    if ( DiffuseTex > -1 && DiffuseTexSampler > -1 )
    {
        lightColor = cColor.m_diffuse * MoveLinearSpace( Tex2D[DiffuseTex].Sample( Samplers[DiffuseTexSampler], input.texcoord ) );
    }
#else
	float4 lightColor = cColor.m_diffuse * MoveLinearSpace( DiffuseTex.Sample( DiffuseTexSampler, input.texcoord ) );
#endif
	lightColor += cColor.m_specular * MoveLinearSpace( Specular );

	return float4( lightColor.rgb, 1.f );
}