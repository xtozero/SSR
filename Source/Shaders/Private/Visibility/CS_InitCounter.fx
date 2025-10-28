#include "Visibility/VisibilityCommon.fxh"

RWStructuredBuffer<uint> Counter;
RWByteAddressBuffer IndirectArgs;

uint NumDrawCallIds;

[numthreads( 32, 1, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint drawCallId = DTid.x;

    if ( drawCallId < NumDrawCallIds )
    {
        Counter[drawCallId] = 0;

        // Dispatch( uint, uint, uint );
        IndirectArgs.Store( drawCallId * IndirectArgsStride + 0, 0 );
        IndirectArgs.Store( drawCallId * IndirectArgsStride + 4, 1 );
        IndirectArgs.Store( drawCallId * IndirectArgsStride + 8, 1 );
    }
}