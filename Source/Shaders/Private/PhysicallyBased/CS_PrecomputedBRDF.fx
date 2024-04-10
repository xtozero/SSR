#include "Common/PBR.fxh"

RWTexture2D<float2> Precomputed;

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadId )
{
    uint2 dims = (uint2)0;
    Precomputed.GetDimensions( dims.x, dims.y );

    if ( all( DTid < dims ) )
    {
        float2 uv = ( (float2)DTid.xy + 0.5f ) / dims;
        Precomputed[DTid.xy] = IntegrateBRDF( uv.x, uv.y );
    }
}