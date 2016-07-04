struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 shadowCoord : TEXCOORD1;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	float depth = input.shadowCoord.z / input.shadowCoord.w;
	return float4( depth.xxx, 1.0f );
}