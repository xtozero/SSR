#ifndef BlockSize
#define BlockSize 256
#endif

#ifndef LaneSize
#define LaneSize 32
#endif

#ifndef MaxSpin
#define MaxSpin 2048
#endif

RWStructuredBuffer<uint> Input;

RWStructuredBuffer<uint> BlockId;
globallycoherent RWStructuredBuffer<uint2> BlockStatus;

RWStructuredBuffer<uint> Output;

uint NumItems;

groupshared uint BlockIndex;
groupshared uint WaveSumPrefixSum[LaneSize];
groupshared uint BlockPrefixSum[BlockSize];
groupshared uint PrefixSum;

void BlockScan( uint localIndex, uint globalIndex )
{
    uint value = ( globalIndex < NumItems ) ? Input[globalIndex] : 0;

    uint laneIndex = WaveGetLaneIndex();
    uint waveIndex = localIndex / LaneSize;

    uint waveSum = WaveActiveSum( value );

    if ( laneIndex == 0 )
    {
        WaveSumPrefixSum[waveIndex] = waveSum;
    }

    GroupMemoryBarrierWithGroupSync();

    if ( waveIndex == 0 )
    {
        uint numWaves = ( BlockSize + LaneSize - 1 ) / LaneSize;

        uint waveSumToScan = ( laneIndex < numWaves ) ? WaveSumPrefixSum[laneIndex] : 0;

        WaveSumPrefixSum[laneIndex] = WavePrefixSum( waveSumToScan );
    }

    GroupMemoryBarrierWithGroupSync();

    uint wavePrefixSum = WavePrefixSum( value );
    BlockPrefixSum[localIndex] = WaveSumPrefixSum[waveIndex] + wavePrefixSum;

    GroupMemoryBarrierWithGroupSync();
}

[numthreads( BlockSize, 1, 1 )]
void main( uint GTid : SV_GroupThreadID
    , uint Gid : SV_GroupID )
{
    if ( GTid == 0 )
    {
        InterlockedAdd( BlockId[0], 1, BlockIndex );
    }

    GroupMemoryBarrierWithGroupSync();

    uint blockIndex = BlockIndex;
    uint localIndex = GTid;
    uint globalIndex = blockIndex * BlockSize + localIndex;

    BlockScan( localIndex, globalIndex );

    uint lastIndex = blockIndex * BlockSize + BlockSize - 1;
    uint back = ( lastIndex < NumItems ) ? Input[lastIndex] : 0;
    uint blockSum = BlockPrefixSum[BlockSize - 1] + back;

    if ( localIndex == 0 )
    {
        PrefixSum = 0;
        if ( blockIndex > 0 )
        {
            uint prevBlockIndex = blockIndex - 1;
            uint2 status = uint2( 0, 0 );

            int i = 0;
            while ( ( i < MaxSpin ) && ( status = BlockStatus[prevBlockIndex] ).y != 1 )
            {
                i += 1;
            }

            PrefixSum = status.x;
            BlockStatus[blockIndex].x = blockSum + PrefixSum;
            DeviceMemoryBarrier();
            BlockStatus[blockIndex].y = 1;
        }
        else
        {
            BlockStatus[blockIndex].x = blockSum;
            DeviceMemoryBarrier();
            BlockStatus[blockIndex].y = 1;
        }
    }

    GroupMemoryBarrierWithGroupSync();

    if ( globalIndex < NumItems )
    {
        Output[globalIndex] = BlockPrefixSum[localIndex] + PrefixSum;
    }
}