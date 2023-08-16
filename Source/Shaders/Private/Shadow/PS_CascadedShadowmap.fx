#include "Common/LightCommon.fxh"
#include "Common/NormalCompression.fxh"
#include "Shadow/ShadowConstant.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
#if EnableRSMs == 1
	float3 worldPos : POSITION0;
	float3 normal : NORMAL;
#endif
	float2 shadowCoord : TEXCOORD0;
	uint rtIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUTPUT
{
	float depth : SV_TARGET0;
#if EnableRSMs == 1
	float3 worldPos : SV_TARGET1;
	float3 packedNormal : SV_TARGET2;
	float4 flux : SV_TARGET3;
#endif
};

PS_OUTPUT main( PS_INPUT input ) : SV_TARGET
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	
	output.depth = input.shadowCoord.x / input.shadowCoord.y;
#if EnableRSMs == 1
	output.worldPos = input.worldPos;
	float3 enc = SignedOctEncode( normalize( input.normal ) );
	output.packedNormal = float4( 0.f, enc );
	output.flux = Diffuse * GetLight( LightIdx ).m_diffuse;
#endif

	return output;
}