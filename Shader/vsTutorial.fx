cbuffer VEIW_PROJECTION : register( b0 )
{
	matrix g_viewMatrix : packoffset( c0 );
	matrix g_projectionMatrix : packoffset( c4 );
};

struct VS_INPUT
{
	float3 position : POSITION;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix viewPorjection = mul( g_viewMatrix, g_projectionMatrix );

	output.position = mul( float4( input.position, 1.0f ), viewPorjection );

	return output;
}