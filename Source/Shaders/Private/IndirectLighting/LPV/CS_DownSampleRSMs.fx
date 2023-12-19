#include "Common/NormalCompression.fxh"
#include "IndirectLighting/LPV/LPVCommon.fxh"

cbuffer DownSampleRSMsParameters : register( b0 )
{   
    uint3 RSMsDimensions;
    uint KernelSize;
    float3 ToLightDir;
};

Texture2DArray RSMsWorldPosition : register( t0 );
Texture2DArray RSMsNormal : register( t1 );
Texture2DArray RSMsFlux : register( t2 );

RWTexture2DArray<float4> OutRSMsWorldPosition : register( u0 );
RWTexture2DArray<float4> OutRSMsNormal : register( u1 );
RWTexture2DArray<float4> OutRSMsFlux : register( u2 );

float Luminance( float3 color )
{
    return 0.2126f * color.r + 0.7152 * color.g + 0.0722f * color.b;
}

float TexelLuminance( float3 flux, float3 normal )
{
    return Luminance( flux ) * saturate( dot( ToLightDir, normal ) );
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if ( all( DTid < RSMsDimensions ) )
    {
        float maxLuminance = -1.f;
        uint4 brightestTexelUV;

        for ( int y = 0; y < KernelSize; ++y )
        {
            for ( int x = 0; x < KernelSize; ++x )
            {
                uint4 uv = { DTid.x * KernelSize + x, DTid.y * KernelSize + y, DTid.z, 0 };

                float3 packedNormal = RSMsNormal.Load( uv ).yzw;
                float3 sampleNormal = SignedOctDecode( packedNormal );

                float3 sampleflux = RSMsFlux.Load( uv ).rgb;

                float sampleLuminance = TexelLuminance( sampleflux, sampleNormal );
                if ( maxLuminance < sampleLuminance )
                {
                    maxLuminance = sampleLuminance;
                    brightestTexelUV = uv;
                }
            }
        }

        float3 brightestPosition = RSMsWorldPosition.Load( brightestTexelUV ).xyz;
        int3 brightestGridPos = GetGridPos( brightestPosition );

        float numSample = 0.f;
        float3 position = 0.f;
        float3 normal = 0.f;
        float3 flux = 0.f;

        for ( int y = 0; y < KernelSize; ++y )
        {
            for ( int x = 0; x < KernelSize; ++x )
            {
                uint4 uv = { DTid.x * KernelSize + x, DTid.y * KernelSize + y, DTid.z, 0 };

                float3 samplePosition = RSMsWorldPosition.Load( uv ).xyz;
                int3 sampleGridPos = GetGridPos( samplePosition );
                int3 dGrid = sampleGridPos - brightestGridPos;

                if ( dot( dGrid, dGrid ) < 3 )
                {
                    float3 packedNormal = RSMsNormal.Load( uv ).yzw;
                    float3 sampleNormal = SignedOctDecode( packedNormal );

                    float3 sampleFlux = RSMsFlux.Load( uv ).rgb;

                    position += samplePosition;
                    normal += sampleNormal;
                    flux += sampleFlux;

                    ++numSample;
                }
            }
        }

        float numerator = 1.f / numSample;
        OutRSMsNormal[DTid] = float4( 0.f, SignedOctEncode( normal * numerator ) );
        OutRSMsWorldPosition[DTid] = float4( position * numerator, 0.f );
        OutRSMsFlux[DTid] = float4( flux * numerator, 0.f );
    }
}