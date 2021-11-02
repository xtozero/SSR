#include "ViewConstant.fxh"
#include "vsCommon.fxh"

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
	output.position = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix );
	output.position = mul( float4( output.position.xyz, 1.0f ), g_viewMatrix );
	output.position = mul( float4( output.position.xyz, 1.0f ), g_projectionMatrix );
	
	return output;
}