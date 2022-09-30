#include "Constants.fxh"
#include "ViewConstant.fxh"

struct PrimitiveSceneData
{
	matrix m_worldMatrix;
	matrix m_prevWorldMatrix;
	matrix m_invWorldMatrix;
};

static const int PRIMITIVE_SCENE_DATA_STRIDE = 12;

StructuredBuffer<float4> primitiveInfo : register( t0 );

PrimitiveSceneData GetPrimitiveData( uint primitiveId )
{
	PrimitiveSceneData primitiveData; 
	uint baseOffset = primitiveId * PRIMITIVE_SCENE_DATA_STRIDE;

	primitiveData.m_worldMatrix[0] = primitiveInfo[baseOffset + 0];
	primitiveData.m_worldMatrix[1] = primitiveInfo[baseOffset + 1];
	primitiveData.m_worldMatrix[2] = primitiveInfo[baseOffset + 2];
	primitiveData.m_worldMatrix[3] = primitiveInfo[baseOffset + 3];

	primitiveData.m_prevWorldMatrix[0] = primitiveInfo[baseOffset + 4];
	primitiveData.m_prevWorldMatrix[1] = primitiveInfo[baseOffset + 5];
	primitiveData.m_prevWorldMatrix[2] = primitiveInfo[baseOffset + 6];
	primitiveData.m_prevWorldMatrix[3] = primitiveInfo[baseOffset + 7];

	primitiveData.m_invWorldMatrix[0] = primitiveInfo[baseOffset + 8];
	primitiveData.m_invWorldMatrix[1] = primitiveInfo[baseOffset + 9];
	primitiveData.m_invWorldMatrix[2] = primitiveInfo[baseOffset + 10];
	primitiveData.m_invWorldMatrix[3] = primitiveInfo[baseOffset + 11];

	return primitiveData;
}	

float4 ApplyTAAJittering( float4 clipSpace )
{
#if TAA == 1
	int idx = FrameCount % MAX_HALTON_SEQUENCE;

	float2 jitter = HALTON_SEQUENCE[idx];
	jitter.x = ( jitter.x - 0.5f ) / ViewportDimensions.x * 2.f;
	jitter.y = ( jitter.y - 0.5f ) / ViewportDimensions.y * 2.f;

	clipSpace.xy += jitter * clipSpace.w;
#endif

	return clipSpace;
}