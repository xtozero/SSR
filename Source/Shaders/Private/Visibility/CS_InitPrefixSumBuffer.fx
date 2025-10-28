RWStructuredBuffer<uint> BlockId;
RWStructuredBuffer<uint2> BlockStatus;

uint NumBlocks;

[numthreads( 32, 1, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x;

    if ( index == 0 )
    {
        BlockId[index] = 0;
    }

    if ( index < NumBlocks )
    {
        BlockStatus[index] = uint2( 0, 0 );
    }
}