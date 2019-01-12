cbuffer VIEW_PORJECTION : register(b1)
{
    matrix g_viewMatrix : packoffset(c0);
    matrix g_projectionMatrix : packoffset(c4);
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT vsMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.pos = mul( float4( input.pos, 1.f ), g_viewMatrix );
	output.pos = mul( output.pos, g_projectionMatrix );
    output.color = input.color;
    return output;
}

float4 psMain( PS_INPUT input ) : SV_TARGET
{
    return input.color;
}