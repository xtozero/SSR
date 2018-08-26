#define MAX_CASCADED_NUM 2

cbuffer LIGHT_VIEW_PROJECTION : register(b0)
{
	float g_bias;
	float3 padding;
	matrix g_lightView[MAX_CASCADED_NUM];
	matrix g_lightProjection[MAX_CASCADED_NUM];
};

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
			float4 viewPosition = mul( input[j].position, g_lightView[i] );
			viewPosition.z += g_bias;
			
			output.position = mul( viewPosition, g_lightProjection[i] );
			output.shadowCoord = output.position.zw;
			output.rtIndex = i;
			triStream.Append( output );
		}
		triStream.RestartStrip();
	}
}