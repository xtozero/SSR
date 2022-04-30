#include "Common/TexCommon.fxh"

Texture2D ssrTex : register(t1);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	return ssrTex.Sample( baseSampler, input.texcoord );
}