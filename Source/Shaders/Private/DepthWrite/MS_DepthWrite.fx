#include "Common/MsCommon.fxh"

StructuredBuffer<float3> Positions;
StructuredBuffer<float3> Normals;

struct MS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 curFramePosition : POSITION0;
	float4 prevFramePosition : POSITION1;
	float3 viewPosition : POSITION2;
	float3 worldNormal : NORMAL;
};

[outputtopology( "triangle" )]
[numthreads(128, 1, 1)]
void main( uint GTid : SV_GroupThreadID
	, uint Gid : SV_GroupID
	, in payload Payload payload
	, out indices uint3 triangles[128]
	, out vertices MS_OUTPUT vertices[64] )
{
	uint meshletIndex = payload.m_meshletIndices[Gid];
	uint primitiveIdOffset = payload.m_primitiveIdIndices[Gid];

	Meshlet meshlet = Meshlets[meshletIndex];
	SetMeshOutputCounts( meshlet.m_vertexCount, meshlet.m_triangleCount );

	if ( GTid < meshlet.m_triangleCount )
	{
		uint packed = TriangleIndices[meshlet.m_triangleOffset + GTid];
		uint idx0 = packed & 0xFF;
		uint idx1 = ( packed >> 8 ) & 0xFF;
		uint idx2 = ( packed >> 16 ) & 0xFF;
		triangles[GTid] = uint3( idx0, idx1, idx2 );
	}

	if ( GTid < meshlet.m_vertexCount )
	{
		uint vertexIndex = VertexIndices[meshlet.m_vertexOffset + GTid];
		
		PrimitiveSceneData primitiveData = GetPrimitiveData( PrimitiveIds[primitiveIdOffset] );
		float4 curFramePosition = mul( float4( Positions[vertexIndex], 1.f ), primitiveData.m_worldMatrix );
		float3 viewPosition = mul( float4( curFramePosition.xyz, 1.f ), ViewMatrix ).xyz;
		curFramePosition = mul( float4( viewPosition, 1.f ), ProjectionMatrix );

        vertices[GTid].curFramePosition = curFramePosition;
        vertices[GTid].viewPosition = viewPosition;
        vertices[GTid].worldNormal = mul( float4( Normals[vertexIndex], 0.f ), transpose( primitiveData.m_invWorldMatrix ) ).xyz;
		
        float4 prevFramePosition = mul( float4( Positions[vertexIndex], 1.f ), primitiveData.m_prevWorldMatrix );
        prevFramePosition = mul( float4( prevFramePosition.xyz, 1.f ), PrevViewMatrix );
        prevFramePosition = mul( float4( prevFramePosition.xyz, 1.f ), PrevProjectionMatrix );

        vertices[GTid].prevFramePosition = prevFramePosition;

        vertices[GTid].position = ApplyTAAJittering( curFramePosition );
	}
}