struct VS_OUTPUT
{
    uint vertexId : VERTEXID;
};

VS_OUTPUT main( uint vertexId : SV_VertexID )
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.vertexId = vertexId;
    
    return output;
}