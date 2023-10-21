#include "Common/Constants.fxh"

TextureCube CubeMap : register( t0 );
SamplerState LinearSampler : register( s0 );

static const float SampleDelta = 0.025f;

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 localPosition : POSITION0;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 normal = normalize( input.localPosition );

    float3 up = ( abs( normal.y ) < 0.999 ) ? float3( 0.f, 1.f, 0.f ) : float3( 0.f, 0.f, 1.f );
    float3 right = normalize( cross( up, normal ) );
    up = normalize( cross( normal, right ) );
    
    float3x3 toWorld = float3x3( right, up, normal );

    float3 irradiance = 0.f;

    float numSample = 0.f; 
    for ( float phi = 0.f; phi < 2.f * PI; phi += SampleDelta )
    {
        for ( float theta = 0.f; theta < 0.5f * PI; theta += SampleDelta )
        {
            float3 tangentSample = float3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) );
            float3 worldSample = normalize( mul( tangentSample, toWorld ) );

            irradiance += CubeMap.Sample( LinearSampler, worldSample ).rgb * cos( theta ) * sin( theta );

            ++numSample;
        }
    }
    irradiance = PI * irradiance / numSample;

    return float4( irradiance, 1.f );
}