#include "ShadowConstant.fxh"

struct GS_INPUT
{
	float4 position : POSITION;
};

struct GS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 shadowCoord : TEXCOORD0;
	uint rtIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(3 * MAX_CASCADED_NUM)]
void main( triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> triStream )
{
	GS_OUTPUT output = (GS_OUTPUT)0;
	
	for ( int i = 0; i < MAX_CASCADED_NUM; ++i)
	{
		for ( int j = 0; j < 3; ++j )
		{
			output.position = mul( input[j].position, ShadowViewProjection[i] );
			output.shadowCoord = output.position.zw;
			output.rtIndex = i;
			triStream.Append( output );
		}
		triStream.RestartStrip();
	}
}