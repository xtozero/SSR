#include "Visibility/VisibilityCommon.fxh"

Texture2D<uint> Visibility;
RWStructuredBuffer<uint> Counter;

uint2 ScreenSize;

[numthreads( 8, 4, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if ( all( DTid.xy < ScreenSize ) )
    {
        uint triangleIndex = 0;
        uint drawcallId = 0;
        DecodeVisibility( Visibility.Load( int3( DTid.xy, 0 ) ), triangleIndex, drawcallId );

        if ( drawcallId > 0 )
        {
            uint4 mask = WaveMatch( drawcallId );
            uint sameCount = CountBits( mask );
            uint firstLane = GetLowestLane( mask );

            if ( WaveGetLaneIndex() == firstLane )
            {
                InterlockedAdd( Counter[drawcallId], sameCount );
            }
        }
    }
}