#include "texCommon.fxh"

Texture2D ssrTex : register(t1);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	//Test
	float4 color = ssrTex.Sample( baseSampler, input.texcoord );

	if ( color.a == 0 )
	{
		discard;
	}

	return color;
}