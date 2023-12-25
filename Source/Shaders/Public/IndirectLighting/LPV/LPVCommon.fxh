#include "SH/SphericalHarmonics.fxh"

cbuffer LPVCommonParameters : register( b5 )
{
    float4 TexDimension;
    float4 CellSize;
};

int3 GetGridPos( float3 position )
{
    int3 halfTexDimension = int3( TexDimension.xyz * 0.5f );

    return ( position / CellSize.xyz ) + halfTexDimension;
}

float3 GetLpvUV( float3 position )
{
    float3 halfTexDimension = TexDimension.xyz * 0.5f;
    float3 gridPosition = ( position / CellSize.xyz ) + halfTexDimension;
    
    return gridPosition / TexDimension.xyz;
}

float4 CosineLobe( float3 normal )
{
    static const float SHCosineLobeC0 = 0.886226925f; // sqrt( pi ) / 2
    static const float SHCosineLobeC1 = 1.02332671f; // sqrt( pi / 3 )

    return float4( SHCosineLobeC0, -SHCosineLobeC1 * normal.y, SHCosineLobeC1 * normal.z, -SHCosineLobeC1 * normal.x );
}
