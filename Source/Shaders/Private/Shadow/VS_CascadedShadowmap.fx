#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

struct VS_INPUT
{
	float3 position : POSITION;
#if EnableRSMs == 1
	float3 normal : NORMAL;
#endif
	uint primitiveId : PRIMITIVEID;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
#if EnableRSMs == 1
	float3 normal : NORMAL;
#endif
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	PrimitiveSceneData primitiveData = GetPrimitiveData( input.primitiveId );
	output.position = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix );

#if EnableRSMs == 1
	output.normal = mul( float4( input.normal, 0.f ), transpose( primitiveData.m_invWorldMatrix ) ).xyz;
#endif

	return output;
}