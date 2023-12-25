#include "Common/Constants.fxh"
#include "Common/NormalCompression.fxh"
#include "Common/ViewConstant.fxh"
#include "IndirectLighting/LPV/LPVCommon.fxh"

Texture2D ViewSpaceDistance : register( t0 );
Texture2D WorldNormal : register( t1 );

Texture3D CoeffR : register( t2 );
Texture3D CoeffG : register( t3 );
Texture3D CoeffB : register( t4 );

SamplerState BlackBorderSampler : register( s0 );

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 viewRay : POSITION1;
    float3 worldRay : POSITION2;
    float2 uv : TEXCOORD0;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
    float3 packedNormal = WorldNormal.Sample( BlackBorderSampler, input.uv ).yzw;
    float3 worldNormal = SignedOctDecode( packedNormal );

    float4 sh = ShFunctionL1( -worldNormal );

    float viewSpaceDistance = ViewSpaceDistance.Sample( BlackBorderSampler, input.uv ).x;
    float3 viewPosition = normalize( input.viewRay ) * viewSpaceDistance;

    float4 worldPosition = mul( float4( viewPosition, 1 ), InvViewMatrix );
    worldPosition /= worldPosition.w;

    float3 lpvUV = GetLpvUV( worldPosition.xyz );

    float4 coeffR = CoeffR.Sample( BlackBorderSampler, lpvUV );
    float4 coeffG = CoeffG.Sample( BlackBorderSampler, lpvUV );
    float4 coeffB = CoeffB.Sample( BlackBorderSampler, lpvUV );

    float3 lighting = max( float3( dot( coeffR, sh ), dot( coeffG, sh ), dot( coeffB, sh ) ), 0.f );

    return float4( lighting / PI, 1.f );
}