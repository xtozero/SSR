struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUTPUT main( uint vertexId : SV_VertexID )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.uv = float2( ( vertexId << 1 ) & 2, vertexId & 2 );
	output.position = float4( output.uv * float2( 2, -2 ) + float2( -1, 1 ), 0, 1 );

	return output;
}