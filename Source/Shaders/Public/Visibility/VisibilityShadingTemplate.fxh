#include "Visibility/VisibilityCommon.fxh"

Texture2D<uint> Visibility;
StructuredBuffer<uint> Counter;
StructuredBuffer<uint> Offset;
StructuredBuffer<uint> WorkList;
StructuredBuffer<uint> PrimitiveIds;

uint DrawCallId;
uint2 ScreenSize;
uint BaseIndex;
uint BaseVertex;

Buffer<uint> Indices;
StructuredBuffer<float3> Positions;

RWTexture2D<float4> SceneColor;

struct ShadingContext
{
    BarycentricDeriv m_deriv;

    float3 m_p0;
    float3 m_p1;
    float3 m_p2;

    uint3 m_indices;

    PrimitiveSceneData m_primitiveData;

    uint2 m_pixelPosition;
    float2 m_screenUV;
};

void ShadingMain( ShadingContext ctx );

[numthreads(32, 1, 1)]
void main( uint DTid : SV_DispatchThreadID )
{
    uint count = Counter[DrawCallId];
    if ( DTid >= count )
    {
        return;
    }

    ShadingContext ctx = (ShadingContext)0;

    uint workListIndex = Offset[DrawCallId] + DTid;
    DecodePixelPosition( WorkList[workListIndex], ctx.m_pixelPosition );

    uint visibility = Visibility[ctx.m_pixelPosition];

    uint triangleIndex = 0;
    uint drawcallId = 0;
    DecodeVisibility( visibility, triangleIndex, drawcallId );

    uint indexBufferBaseOffset = BaseIndex + triangleIndex * 3;

    uint indexBufferOffset0 = indexBufferBaseOffset + 0;
    uint indexBufferOffset1 = indexBufferBaseOffset + 1;
    uint indexBufferOffset2 = indexBufferBaseOffset + 2;

    ctx.m_indices = uint3(
        BaseVertex + Indices[indexBufferOffset0],
        BaseVertex + Indices[indexBufferOffset1],
        BaseVertex + Indices[indexBufferOffset2]
    );
    ctx.m_p0 = Positions[ctx.m_indices.x];
    ctx.m_p1 = Positions[ctx.m_indices.y];
    ctx.m_p2 = Positions[ctx.m_indices.z];

    ctx.m_primitiveData = GetPrimitiveData( PrimitiveIds[DrawCallId] );
    ctx.m_screenUV = ( ctx.m_pixelPosition + 0.5f ) / ScreenSize;
    ctx.m_deriv = ComputePerspectiveBarycentricDeriv(
        ctx.m_p0,
        ctx.m_p1,
        ctx.m_p2,
        ctx.m_primitiveData,
        ViewProjectionMatrix,
        ctx.m_screenUV,
        ScreenSize );

    ShadingMain( ctx );
}