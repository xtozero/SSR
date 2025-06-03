#include "Common/BindlessResources.fxh"
#include "Common/LightCommon.fxh"

#if UseDiffuseTexture == 1
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
#endif

Texture2D SSGITex : register( t3 );
SamplerState SSGITexSampler : register( s3 );

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 projectionPos : POSITION0;
#if UseDiffuseTexture == 1
    float2 texcoord : TEXCOORD;
#endif
};

float4 main( PS_INPUT input ) : SV_Target0
{
    float2 screenUV = ( input.projectionPos.xy / input.projectionPos.w ) * float2( 0.5f, -0.5f ) + 0.5f;
    float4 ssgiColor = MoveLinearSpace( SSGITex.Sample( SSGITexSampler, screenUV ) );

    float4 diffuseColor = (float4)0.f;
#if UseDiffuseTexture == 1
#if SupportsBindless == 1
    if ( DiffuseTex > -1 && DiffuseTexSampler > -1 )
    {
        diffuseColor = MoveLinearSpace( Tex2D[DiffuseTex].Sample( Samplers[DiffuseTexSampler], input.texcoord ) );
    }
#else
	diffuseColor = MoveLinearSpace( DiffuseTex.Sample( DiffuseTexSampler, input.texcoord ) );
#endif
#else
    diffuseColor = MoveLinearSpace( Diffuse );
#endif

    diffuseColor *= ssgiColor;
	return float4( diffuseColor.rgb * diffuseColor.a, 1.f );
}