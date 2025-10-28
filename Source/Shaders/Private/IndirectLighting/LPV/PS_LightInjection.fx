#include "Common/Constants.fxh"
#include "Common/GammaCorrection.fxh"
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
    float3 flux = MoveLinearSpace( float4( input.flux, 1.f ) ).rgb;
    output.coeffR = coeff * flux.r;
    output.coeffG = coeff * flux.g;
    output.coeffB = coeff * flux.b;

    return output;
}