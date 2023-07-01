#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	uint primitiveId : PRIMITIVEID;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 viewPos : POSITION1;
	float3 normal : NORMAL;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	PrimitiveSceneData primitiveData = GetPrimitiveData( input.primitiveId );
	output.worldPos = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix ).xyz;
	output.viewPos = mul( float4( output.worldPos, 1.0f ), ViewMatrix ).xyz;
	output.position = mul( float4( output.viewPos, 1.0f ), ProjectionMatrix );
	output.normal = mul( float4( input.normal, 0.f ), transpose( primitiveData.m_invWorldMatrix ) ).xyz;

	output.position = ApplyTAAJittering( output.position );
	
	return output;
}