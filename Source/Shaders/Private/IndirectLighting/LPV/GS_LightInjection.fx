struct GS_INPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
    float3 flux : FLUX;
    int skip : SKIP;
    int cascadeIndex : CASCADE_INDEX;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 flux : FLUX;
    float surfelWeight : SURFEL_WEIGHT;
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
        output.flux = input[0].flux;
        output.surfelWeight = 0.015f * ( (input[0].cascadeIndex == 0) ? 0.09f : 1.f );
        output.rtIndex = (uint)input[0].position.z;

        pointStream.Append( output );
    }
}