struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

float4 main( PS_INPUT input ) : SV_Target0
{
    return float4( input.color, 1.f );
}