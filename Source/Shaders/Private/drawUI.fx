#include "Common/TexCommon.fxh"

cbuffer VIEW_PORJECTION : register(b1)
{
    matrix g_viewMatrix : packoffset(c0);
    matrix g_projectionMatrix : packoffset(c4);
}

struct VS_INPUT
{
    float2 pos : POSITION;
	float2 texCoord : TEXCOORD;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
    float4 color : COLOR;
};

PS_INPUT vsMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.pos = mul( float4( input.pos, 0, 1.f ), g_projectionMatrix );
	output.texCoord = input.texCoord;
    output.color = input.color;
    return output;
}

float4 psMain( PS_INPUT input ) : SV_TARGET
{
	float4 sampleColor = Sample( input.texCoord );
	sampleColor.a *= sampleColor.x;
    return input.color * sampleColor;
}