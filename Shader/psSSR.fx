#include "lightCommon.fxh"

Texture2D framebufferTex : register( t1 );
Texture2D depthbufferTex : register( t2 );

float4 main( PS_INPUT input ) : SV_TARGET
{
	//Test
	return float4( 1.0f, 0.f, 0.f, 1.f );
}