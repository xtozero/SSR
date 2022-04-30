struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 shadowCoord : TEXCOORD0;
	uint rtIndex : SV_RenderTargetArrayIndex;
};

float main( PS_INPUT input ) : SV_TARGET
{
	float depth = input.shadowCoord.x / input.shadowCoord.y;
	return depth;
}