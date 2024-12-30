#include "PrimitiveSceneData.fxh"
#include "TAACommon.fxh"

#define NumGroupForAS 32

struct Payload
{
	uint m_meshletIndices[NumGroupForAS];
	uint m_primitiveIdIndices[NumGroupForAS];
};

struct Meshlet
{
	uint m_vertexOffset;
	uint m_triangleOffset;

	uint m_vertexCount;
	uint m_triangleCount;

	float3 m_center;
	float m_radius;
};

StructuredBuffer<uint> PrimitiveIds;

StructuredBuffer<Meshlet> Meshlets;
StructuredBuffer<uint> VertexIndices;
StructuredBuffer<uint> TriangleIndices;