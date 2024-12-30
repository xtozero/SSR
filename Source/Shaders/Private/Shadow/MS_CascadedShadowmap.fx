#include "Common/MsCommon.fxh"
#include "Shadow/ShadowConstant.fxh"

StructuredBuffer<float3> Positions;

#if EnableRSMs == 1
StructuredBuffer<float3> Normals;
#endif

struct MS_OUTPUT
{
	float4 position : SV_POSITION;
#if EnableRSMs == 1
	float3 worldPos : POSITION0;
	float3 normal : NORMAL;
#endif
	float2 shadowCoord : TEXCOORD0;
};

struct PrimitiveAttribute
{
    uint rtIndex : SV_RenderTargetArrayIndex;
};

[outputtopology( "triangle" )]
[numthreads(128, 1, 1)]
void main( uint GTid : SV_GroupThreadID
	, uint3 Gid : SV_GroupID
	, in payload Payload payload
	, out indices uint3 triangles[128]
    , out primitives PrimitiveAttribute prims[128]
	, out vertices MS_OUTPUT vertices[64] )
{
	uint meshletIndex = payload.m_meshletIndices[Gid.x];
	uint primitiveIdOffset = payload.m_primitiveIdIndices[Gid.x];
    uint cascadeIndex = Gid.y;

	Meshlet meshlet = Meshlets[meshletIndex];
	SetMeshOutputCounts( meshlet.m_vertexCount, meshlet.m_triangleCount );

	if ( GTid < meshlet.m_triangleCount )
	{
		uint packed = TriangleIndices[meshlet.m_triangleOffset + GTid];
		uint idx0 = packed & 0xFF;
		uint idx1 = ( packed >> 8 ) & 0xFF;
		uint idx2 = ( packed >> 16 ) & 0xFF;
		triangles[GTid] = float3( idx0, idx1, idx2 );
        prims[GTid].rtIndex = cascadeIndex;
	}

	if ( GTid < meshlet.m_vertexCount )
	{
		uint vertexIndex = VertexIndices[meshlet.m_vertexOffset + GTid];
		
		PrimitiveSceneData primitiveData = GetPrimitiveData( PrimitiveIds[primitiveIdOffset] );
		float4 worldPos = mul( float4( Positions[vertexIndex], 1.f ), primitiveData.m_worldMatrix );
        float4 shadowProjectionPos = mul( worldPos, ShadowViewProjection[cascadeIndex] );
        vertices[GTid].position = shadowProjectionPos;

#if EnableRSMs == 1
        vertices[GTid].worldPos = worldPos.xyz;
        vertices[GTid].normal = mul( float4( Normals[vertexIndex], 0.f ), transpose( primitiveData.m_invWorldMatrix ) ).xyz;
#endif

        vertices[GTid].shadowCoord = shadowProjectionPos.zw;
	}
}