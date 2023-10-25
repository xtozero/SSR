#include "Common/Constants.fxh"
#include "SH/SphericalHarmonics.fxh"

TextureCube CubeMap : register( t0 );
SamplerState LinearSampler : register( s0 );
RWStructuredBuffer<float3> Coeffs : register( u0 );

static const int ThreadGroupX = 16;
static const int ThreadGroupY = 16;

static const float3 Black = (float3)0;
static const float SampleDelta = 0.025f;
static const float DeltaPhi = SampleDelta * ThreadGroupX;
static const float DeltaTheta = SampleDelta * ThreadGroupY;

groupshared float3 SharedCoeffs[ThreadGroupX * ThreadGroupY][9];
groupshared int TotalSample;

[numthreads(ThreadGroupX, ThreadGroupY, 1)]
void main( uint3 GTid: SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    if ( GI == 0 )
    {
        TotalSample = 0;
    }

    GroupMemoryBarrierWithGroupSync();

    float3 coeffs[9] = { Black, Black, Black, Black, Black, Black, Black, Black, Black };
    int numSample = 0; 
    for ( float phi = GTid.x * SampleDelta; phi < 2.f * PI; phi += DeltaPhi )
    {
        for ( float theta = GTid.y * SampleDelta; theta < PI; theta += DeltaTheta )
        {
            float3 sampleDir = normalize( float3( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) ) );
            float3 radiance = CubeMap.SampleLevel( LinearSampler, sampleDir, 0 ).rgb;

            float y[9];
            ShFunctionL2( sampleDir, y );

            [unroll]
            for ( int i = 0; i < 9; ++i )
            {
                coeffs[i] += radiance * y[i] * sin( theta );
            }

            ++numSample;
        }
    }

    int sharedIndex = GTid.y * ThreadGroupX + GTid.x;

    [unroll]
    for ( int i = 0; i < 9; ++i )
    {
        SharedCoeffs[sharedIndex][i] = coeffs[i];
        coeffs[i] = Black;
    }

    InterlockedAdd( TotalSample, numSample );

    GroupMemoryBarrierWithGroupSync();

    if ( GI == 0 )
    {
        for ( int i = 0; i < ThreadGroupX * ThreadGroupY; ++i )
        {
            [unroll]
            for ( int j = 0; j < 9; ++j )
            {
                coeffs[j] += SharedCoeffs[i][j];
            }
        }

        float dOmega = 2.f * PI / float( TotalSample );

        [unroll]
        for ( int i = 0; i < 9; ++i )
        {
            Coeffs[i] = coeffs[i] * dOmega;
        }
    }
}