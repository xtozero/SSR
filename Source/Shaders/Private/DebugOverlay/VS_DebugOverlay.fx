#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 viewPosition = mul( float4( input.position, 1.f ), ViewMatrix );
    float4 projectionPosition = mul( viewPosition, ProjectionMatrix );
    
    output.position = ApplyTAAJittering( projectionPosition );
    output.color = input.color;
	
    return output;
}