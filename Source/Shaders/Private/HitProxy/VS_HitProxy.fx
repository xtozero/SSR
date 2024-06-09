#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

struct VS_INPUT
{
	float3 position : POSITION;
	uint primitiveId : PRIMITIVEID;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	PrimitiveSceneData primitiveData = GetPrimitiveData( input.primitiveId );
	float4 worldPosition = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix );
	float4 viewPosition = mul( worldPosition, ViewMatrix );
	float4 projectionPosition = mul( viewPosition, ProjectionMatrix );
	
	output.position = projectionPosition;
	
	return output;
}