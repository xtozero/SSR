struct MS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

[outputtopology( "triangle" )]
[numthreads( 1, 1, 1 )]
void main( out indices uint3 triangles[1], out vertices MS_OUTPUT vertices[3] )
{
    SetMeshOutputCounts( 3, 1 );

    triangles[0] = uint3( 0, 1, 2 );

    vertices[0].position = float4( -0.5f, 0.5f, 0.f, 1.f );
    vertices[0].color = float3( 1.f, 0.f ,0.f );

    vertices[1].position = float4( 0.5f, 0.5f, 0.f, 1.f );
    vertices[1].color = float3( 0.f, 1.f, 0.f );

    vertices[2].position = float4( 0.f, -0.5f, 0.f, 1.f );
    vertices[2].color = float3( 0.f, 0.f, 1.f );
}