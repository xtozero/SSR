#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

StructuredBuffer<uint> DrawCallIds;

uint PrimitiveIdOffset;

struct Input
{
	float3 position : POSITION;
	uint primitiveId : PRIMITIVEID;
};

struct Output
{
	float4 position : SV_POSITION;
	uint drawcallId : DRAWCALLID;
};

Output main( Input input, uint instanceID : SV_InstanceID )
{
	Output output = (Output)0;

	PrimitiveSceneData primitiveData = GetPrimitiveData( input.primitiveId );
	float4 worldPosition = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix );
	float4 viewPosition = mul( worldPosition, ViewMatrix );
	float4 projectPosition = mul( viewPosition, ProjectionMatrix );

	output.position = ApplyTAAJittering( projectPosition );
	output.drawcallId = DrawCallIds[PrimitiveIdOffset + instanceID];

	return output;
}