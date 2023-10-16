TextureCube CubeMap : register( t0 );
SamplerState LinearSampler : register( s0 );

static const float PI = 3.14159265359;

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

    const float sampleDelta = 0.025f;
    float numSample = 0.0; 
    for ( float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta )
    {
        for ( float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta )
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