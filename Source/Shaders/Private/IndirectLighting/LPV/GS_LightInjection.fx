struct GS_INPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
    float3 flux : FLUX;
    int skip : SKIP;
    float surfelArea : SURFEL_AREA;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 flux : FLUX;
    float surfelArea : SURFEL_AREA;
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
        output.surfelArea = input[0].surfelArea;
        output.rtIndex = (uint)input[0].position.z;

        pointStream.Append( output );
    }
}