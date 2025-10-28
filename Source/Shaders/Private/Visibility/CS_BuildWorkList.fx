#include "Visibility/VisibilityCommon.fxh"

Texture2D<uint> Visibility;
RWStructuredBuffer<uint> Offset;

RWByteAddressBuffer IndirectArgs;
RWStructuredBuffer<uint> WorkList;

uint2 ScreenSize;

[numthreads( 8, 4, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint triangleIndex = 0;
    uint drawcallId = 0;
    uint baseIndex = 0;
    bool isValid = false;

    if ( all( DTid.xy < ScreenSize ) )
    {
        DecodeVisibility( Visibility.Load( int3( DTid.xy, 0 ) ), triangleIndex, drawcallId );

        if ( drawcallId > 0 )
        {
            isValid = true;

            uint4 mask = WaveMatch( drawcallId );
            uint sameCount = CountBits( mask );
            uint firstLane = GetLowestLane( mask );

            if ( WaveGetLaneIndex() == firstLane )
            {
                IndirectArgs.InterlockedAdd( drawcallId * IndirectArgsStride, sameCount, baseIndex );
            }
        }
    }

    GroupMemoryBarrierWithGroupSync();

    if ( isValid )
    {
        uint4 mask = WaveMatch( drawcallId );
        uint firstLane = GetLowestLane( mask );

        uint index = GetWaveLocalIndexMatch( mask, WaveGetLaneIndex() );
        index += WaveReadLaneAt( baseIndex, firstLane );
        index += Offset[drawcallId];

        WorkList[index] = EncodePixelPosition( DTid.xy );
    }
}