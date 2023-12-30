struct GS_INPUT
{
	float4 position : SV_POSITION;
    float3 normal : NORMAL;
	float blockingPotencial : BLOCKING_POTENCIAL;
    int skip : SKIP;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float blockingPotencial : BLOCKING_POTENCIAL;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(1)]
void main( point GS_INPUT input[1], inout PointStream<GS_OUTPUT> pointStream )
{
    if ( input[0].skip == 0 )
    {
        GS_OUTPUT output = (GS_OUTPUT)0;

        // output.
        output.position = float4( input[0].position.xy, 0.f, 1.f ); 
        output.normal = input[0].normal;
        output.blockingPotencial = input[0].blockingPotencial;
        output.rtIndex = (uint)input[0].position.z;

        pointStream.Append( output );
    }
}