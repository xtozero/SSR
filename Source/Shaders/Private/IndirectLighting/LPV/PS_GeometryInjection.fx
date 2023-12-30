#include "Common/Constants.fxh"
#include "IndirectLighting/LPV/LPVCommon.fxh"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float blockingPotencial : BLOCKING_POTENCIAL;
    uint rtIndex : SV_RenderTargetArrayIndex;
};

struct PS_OUTPUT
{
    float4 coeffOcclusion : SV_Target0;
};

PS_OUTPUT main( PS_INPUT input )
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    float4 coeff = CosineLobe( input.normal );
    output.coeffOcclusion = coeff * input.blockingPotencial;

    return output;
}