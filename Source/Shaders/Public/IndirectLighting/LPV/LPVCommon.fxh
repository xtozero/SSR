cbuffer LPVCommonParameters : register( b1 )
{
    float4 TexDimension;
    float4 CellSize;
};

int3 GetGridPos( float3 position )
{
    uint halfTexDimension = TexDimension.x * 0.5;
    return ( position / CellSize.xyz ) + int3( halfTexDimension, halfTexDimension, halfTexDimension );
}