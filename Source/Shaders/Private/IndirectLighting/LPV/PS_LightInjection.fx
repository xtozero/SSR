#include "Common/Constants.fxh"
#include "IndirectLighting/LPV/LPVCommon.fxh"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 flux : FLUX;
    float surfelArea : SURFEL_AREA;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUTPUT
{
    float4 coeffR : SV_Target0;
    float4 coeffG : SV_Target1;
    float4 coeffB : SV_Target2;
};

PS_OUTPUT main( PS_INPUT input )
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    float4 coeff = CosineLobe( input.normal ) / PI * input.surfelArea;
    output.coeffR = coeff * input.flux.r;
    output.coeffG = coeff * input.flux.g;
    output.coeffB = coeff * input.flux.b;

    return output;
}