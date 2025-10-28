#include "Visibility/VisibilityCommon.fxh"

RWByteAddressBuffer IndirectArgs;

uint NumDrawCallIds;

[numthreads(32, 1, 1)]
void main( uint DTid : SV_DispatchThreadID )
{
    if ( DTid < NumDrawCallIds )
    {
        uint address = DTid * IndirectArgsStride;
        uint numWork = IndirectArgs.Load( address );
        uint numThreadGroup = ( numWork + 32 - 1 ) / 32;
        IndirectArgs.Store( address, numThreadGroup );
    }
}