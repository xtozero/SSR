RWStructuredBuffer<uint> LaneCount;

[numthreads( 1, 1, 1 )]
void main()
{
    LaneCount[0] = WaveGetLaneCount();
}