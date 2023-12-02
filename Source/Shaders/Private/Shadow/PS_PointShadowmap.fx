#include "Common/LightCommon.fxh"
#include "Shadow/ShadowConstant.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPos : TEXCOORD0;
	uint rtIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUTPUT
{
	float depth : SV_TARGET0;
};

PS_OUTPUT main( PS_INPUT input )
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	
	ForwardLightData light = GetLight( LightIdx );
	output.depth = length( input.worldPos - light.m_position ) / light.m_range;
	return output;
}