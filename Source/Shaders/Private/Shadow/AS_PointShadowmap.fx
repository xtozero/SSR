#include "Common/MsCommon.fxh"

uint MeshletCount;
uint InstanceCount;
uint PrimitiveIdOffset;

groupshared Payload PayloadData;

[numthreads(NumGroupForAS, 1, 1)]
void main( uint DTid : SV_DispatchThreadID
        , uint GTid : SV_GroupThreadID )
{
    bool visible = false;

    uint instanceIndex = DTid / MeshletCount;
    uint meshletIndex = DTid % MeshletCount;

    if ( ( instanceIndex < InstanceCount ) && ( meshletIndex < MeshletCount ) )
    {
        visible = true;
        PayloadData.m_meshletIndices[GTid] = meshletIndex;
        PayloadData.m_primitiveIdIndices[GTid] = instanceIndex + PrimitiveIdOffset;
    }
    
    uint visibleCount = WaveActiveCountBits( visible );
    DispatchMesh( visibleCount, 6, 1, PayloadData );
}