#include "Common/Constants.fxh"
#include "IndirectLighting/LPV/LPVCommon.fxh"

RWTexture3D<float4> CoeffR : register( u0 );
RWTexture3D<float4> CoeffG : register( u1 );
RWTexture3D<float4> CoeffB : register( u2 );

static const float DirectFaceSolidAngle = 0.4006696846f / PI;
static const float SideFaceSolidAngle = 0.4234413544f / PI;

static const float3 NeighborOffsets[6] = {
    float3( 0.f, 0.f, 1.f ),    // +Z
    float3( 0.f, 0.f, -1.f ),   // -Z
    float3( 1.f, 0.f, 0.f ),    // +X
    float3( -1.f, 0.f, 0.f ),   // -X
    float3( 0.f, 1.f, 0.f ),    // +Y
    float3( 0.f, -1.f, 0.f ),   // -Y
};

static const float3x3 NeighborOrientations[6] = {
    float3x3( 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f ), // +Z
    float3x3( -1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f ), // -Z
    float3x3( 0.f, 0.f, -1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f ), // +X
    float3x3( 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f ), // -X
    float3x3( 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f ), // +Y
    float3x3( 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f ), // -Y
};

static const float2 Side[4] = {
    float2( 1.f, 0.f ),
    float2( 0.f, 1.f ),
    float2( -1.f, 0.f ),
    float2( 0.f, -1.f ),
};

float3 GetSideVector( int sideIndex, float3x3 orientation )
{
    static const float smallComponent = 0.447214f; // 1 / sqrt(5)
    static const float bigComponent = 0.894427f; // 2 / sqrt(5)
    return mul( float3( smallComponent * Side[sideIndex], bigComponent ), orientation );
}

float3 GetReprojectionVector( int sideIndex, float3x3 orientation )
{
    return mul( float3( Side[sideIndex], 0.f ), orientation );
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadId )
{
    if ( all( DTid >= 0 ) && all( DTid < TexDimension.xyz ) )
    {
        float4 r = (float4)0.f;
        float4 g = (float4)0.f;
        float4 b = (float4)0.f;

        for ( int neighbor = 0; neighbor < 6; ++neighbor )
        {
            int3 neighborIndex = (int3)DTid - NeighborOffsets[neighbor];

            if ( any( neighborIndex < 0 ) || any( neighborIndex >= TexDimension.xyz ) )
            {
                continue;
            }

            float4 neighborCoeffR = CoeffR[neighborIndex];
            float4 neighborCoeffG = CoeffG[neighborIndex];
            float4 neighborCoeffB = CoeffB[neighborIndex];

            float3x3 orientation = NeighborOrientations[neighbor];

            // side
            for ( int side = 0; side < 4; ++side )
            {
                float3 sideVector = GetSideVector( side, orientation );
                float3 reprojectionVector = GetReprojectionVector( side, orientation );

                float4 sideSH = ShFunctionL1( sideVector );
                float4 reprojectionCosineLobeSH = CosineLobe( reprojectionVector );

                r += SideFaceSolidAngle * dot( neighborCoeffR, sideSH ) * reprojectionCosineLobeSH;
                g += SideFaceSolidAngle * dot( neighborCoeffG, sideSH ) * reprojectionCosineLobeSH;
                b += SideFaceSolidAngle * dot( neighborCoeffB, sideSH ) * reprojectionCosineLobeSH;
            }

            // direct
            float4 directSH = ShFunctionL1( NeighborOffsets[neighbor] );
            float4 reprojectionCosineLobeSH = CosineLobe( NeighborOffsets[neighbor] );

            r += DirectFaceSolidAngle * dot( neighborCoeffR, directSH ) * reprojectionCosineLobeSH;
            g += DirectFaceSolidAngle * dot( neighborCoeffG, directSH ) * reprojectionCosineLobeSH;
            b += DirectFaceSolidAngle * dot( neighborCoeffB, directSH ) * reprojectionCosineLobeSH;
        }

        CoeffR[DTid] += r;
        CoeffG[DTid] += g;
        CoeffB[DTid] += b;
    }
}
