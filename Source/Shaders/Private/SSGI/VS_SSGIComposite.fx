#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
#if UseDiffuseTexture == 1
    float2 texcoord : TEXCOORD;
#endif
	uint primitiveId : PRIMITIVEID;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 projectionPos : POSITION0;
#if UseDiffuseTexture == 1
    float2 texcoord : TEXCOORD;
#endif
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	PrimitiveSceneData primitiveData = GetPrimitiveData( input.primitiveId );
	float3 worldPos = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix ).xyz;
	float3 viewPos = mul( float4( worldPos, 1.0f ), ViewMatrix ).xyz;
	output.projectionPos = mul( float4( viewPos, 1.0f ), ProjectionMatrix );
#if UseDiffuseTexture == 1
    output.texcoord = input.texcoord;
#endif

	output.position = ApplyTAAJittering( output.projectionPos );
	
	return output;
}