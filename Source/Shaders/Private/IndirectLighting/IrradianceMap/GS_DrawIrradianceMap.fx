struct GS_INPUT
{
    uint vertexId : VERTEXID;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 localPosition : POSITION0;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

static const float4 projectedPos[] =
{
    { -1, -1, 0, 1 },
    { -1, 1, 0, 1 },
    { 1, -1, 0, 1 },
    { 1, 1, 0, 1 }
};
    
static const float3 vertices[] =
{
    { -1, -1, -1 },
    { -1, 1, -1 },
    { 1, -1, -1 },
    { 1, 1, -1 },
    { -1, -1, 1 },
    { -1, 1, 1 },
    { 1, -1, 1 },
    { 1, 1, 1 }
};
    
static const int4 indices[] =
{
    { 6, 7, 2, 3 },
    { 0, 1, 4, 5 },
    { 5, 1, 7, 3 },
    { 0, 4, 2, 6 },
    { 4, 5, 6, 7 },
    { 2, 3, 0, 1 }
};

[maxvertexcount(4)]
void main( point GS_INPUT input[1], inout TriangleStream<GS_OUTPUT> triStream )
{
    GS_OUTPUT output = (GS_OUTPUT)0;
    output.rtIndex = input[0].vertexId;
    
    for ( int i = 0; i < 4; ++i )
    {
        output.position = projectedPos[i];
        int index = indices[input[0].vertexId][i];
        output.localPosition = vertices[index];
        triStream.Append( output );
    }
    triStream.RestartStrip();
}