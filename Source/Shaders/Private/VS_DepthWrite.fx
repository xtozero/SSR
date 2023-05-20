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
	float4 curFramePosition : POSITION0;
	float4 prevFramePosition : POSITION1;
	float3 viewPosition : POSITION2;
	float3 worldNormal : NORMAL;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	PrimitiveSceneData primitiveData = GetPrimitiveData( input.primitiveId );
	output.curFramePosition = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix );
	output.viewPosition = mul( float4( output.curFramePosition.xyz, 1.0f ), ViewMatrix );
	output.curFramePosition = mul( float4( output.viewPosition, 1.0f ), ProjectionMatrix );
	output.worldNormal = mul( float4( input.normal, 0.f ), transpose( primitiveData.m_invWorldMatrix ) ).xyz;
	
	output.prevFramePosition = mul( float4( input.position, 1.0f ), primitiveData.m_prevWorldMatrix );
	output.prevFramePosition = mul( float4( output.prevFramePosition.xyz, 1.0f ), PrevViewMatrix );
	output.prevFramePosition = mul( float4( output.prevFramePosition.xyz, 1.0f ), PrevProjectionMatrix );

	output.position = ApplyTAAJittering( output.curFramePosition );
	
	return output;
}