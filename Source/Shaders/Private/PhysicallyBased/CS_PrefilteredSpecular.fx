#include "Common/PBR.fxh"

cbuffer PrefilterSpecularParameter : register( b0 )
{
    float Roughness;
};

TextureCube EnvMap;
SamplerState EnvMapSampler;
RWTexture2DArray<float4> Prefiltered;

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadId )
{
    uint3 dims = (uint3)0;
    Prefiltered.GetDimensions( dims.x, dims.y, dims.z );

    if ( all( DTid < dims ) )
    {
        float2 uv = float2( DTid.xy + 0.5f ) / dims.xy;
        uv = ( uv - 0.5f ) * float2( 2.f, -2.f );

        // https://learn.microsoft.com/en-us/windows/win32/direct3d9/cubic-environment-mapping
        float3 normal = 0;
        if ( DTid.z == 0 ) // +x
        {
            normal = float3( 1.f, uv.y, -uv.x );
        }
        else if ( DTid.z == 1 ) // -x
        {
            normal = float3( -1.f, uv.y, uv.x );
        }
        else if ( DTid.z == 2 ) // +y
        {
            normal = float3( uv.x, 1.f, -uv.y );
        }
        else if ( DTid.z == 3 ) // -y
        {
            normal = float3( uv.x, -1.f, uv.y );
        }
        else if ( DTid.z == 4 ) // +z
        {
            normal = float3( uv.x, uv.y, 1.f );
        }
        else // -z
        {
            normal = float3( -uv.x, uv.y, -1.f );
        }

        normal = normalize( normal );    
        float3 prefilteredSpecular = PrefilterSpecular( normal, Roughness, EnvMap, EnvMapSampler );
        Prefiltered[DTid] = float4( prefilteredSpecular, Roughness );
    }
}