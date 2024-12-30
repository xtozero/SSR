#include "Common/Frustum.fxh"
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
    uint instanceIdOffset = instanceIndex + PrimitiveIdOffset;

    if ( ( instanceIndex < InstanceCount ) && ( meshletIndex < MeshletCount ) )
    {
        PrimitiveSceneData primitiveData = GetPrimitiveData( PrimitiveIds[instanceIdOffset] );
        Frustum frustum = ExtractFrustum( mul( primitiveData.m_worldMatrix, mul( ViewMatrix, ProjectionMatrix ) ) );

        visible = IsInside( frustum, Meshlets[meshletIndex].m_center, Meshlets[meshletIndex].m_radius );
    }

    if (visible)
    {
        uint visibleIndex = WavePrefixCountBits( visible );

        PayloadData.m_meshletIndices[visibleIndex] = meshletIndex;
        PayloadData.m_primitiveIdIndices[visibleIndex] = instanceIdOffset;
    }
    
    uint visibleCount = WaveActiveCountBits( visible );
    DispatchMesh( visibleCount, 1, 1, PayloadData );
}