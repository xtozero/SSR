struct PrimitiveSceneData
{
	matrix m_worldMatrix;
	matrix m_invWorldMatrix;
};

static const int PRIMITIVE_SCENE_DATA_STRIDE = 8;

StructuredBuffer<float4> primitiveInfo : register( t0 );

PrimitiveSceneData GetPrimitiveData( uint primitiveId )
{
	PrimitiveSceneData primitiveData; 
	uint baseOffset = primitiveId * PRIMITIVE_SCENE_DATA_STRIDE;

	primitiveData.m_worldMatrix[0] = primitiveInfo[baseOffset + 0];
	primitiveData.m_worldMatrix[1] = primitiveInfo[baseOffset + 1];
	primitiveData.m_worldMatrix[2] = primitiveInfo[baseOffset + 2];
	primitiveData.m_worldMatrix[3] = primitiveInfo[baseOffset + 3];

	primitiveData.m_invWorldMatrix[0] = primitiveInfo[baseOffset + 4];
	primitiveData.m_invWorldMatrix[1] = primitiveInfo[baseOffset + 5];
	primitiveData.m_invWorldMatrix[2] = primitiveInfo[baseOffset + 6];
	primitiveData.m_invWorldMatrix[3] = primitiveInfo[baseOffset + 7];

	return primitiveData;
}

cbuffer SHADOW_VIEW_PROJECTION : register(b3)
{
	float g_zBias;
	float3 padding;
	matrix g_lightViewMatrix;
	matrix g_lightProjectionMatrix;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	uint primitiveId : PRIMITIVEID;
};
