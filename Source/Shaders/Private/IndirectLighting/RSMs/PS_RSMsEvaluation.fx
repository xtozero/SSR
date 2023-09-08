#include "Common/NormalCompression.fxh"
#include "Common/ViewConstant.fxh"
#include "Shadow/ShadowCommon.fxh"

Texture2D ViewSpaceDistance : register( t0 );
Texture2D WorldNormal : register( t1 );

Texture2DArray RSMsNormal : register( t2 );
Texture2DArray RSMsWorldPosition : register( t3 );
Texture2DArray RSMsFlux : register( t4 );

Buffer<float3> SamplingPattern : register( t5 );

cbuffer RSMsConstantParameters : register( b3 )
{
    uint NumSamplingPattern;
    float MaxRadius;
}

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
    float viewSpaceDistance = ViewSpaceDistance.Sample( BlackBorderSampler, input.uv ).x;
    float3 viewPosition = normalize( input.viewRay ) * viewSpaceDistance;

    float4 worldPosition = mul( float4( viewPosition, 1 ), InvViewMatrix );
    worldPosition /= worldPosition.w;

    int cascadeIndex = SearchCascadeIndex( viewPosition.z );	
	float4 shadowCoord = mul( worldPosition, ShadowViewProjection[cascadeIndex] );
	
	float2 shadowMapUV = shadowCoord.xy / shadowCoord.w;
	shadowMapUV.xy = shadowMapUV * float2( 0.5f, -0.5f ) + 0.5f;

    float3 packedNormal = WorldNormal.Sample( BlackBorderSampler, input.uv ).yzw;
    float3 worldNormal = SignedOctDecode( packedNormal );

    float cascadeScale = CascadeConstants[0].m_zFar / CascadeConstants[cascadeIndex].m_zFar;
    float3 indirectLight = 0.f;
	[loop]
    for ( uint i = 0; i < NumSamplingPattern; ++i )
    {
        float3 samplingOffset = SamplingPattern[i];
        float3 uv = float3( shadowMapUV + MaxRadius * cascadeScale * samplingOffset.xy, cascadeIndex );

        float3 positionP = RSMsWorldPosition.SampleLevel( BlackBorderSampler, uv, 0 ).xyz;
        float3 normalP = SignedOctDecode( RSMsNormal.SampleLevel( BlackBorderSampler, uv, 0 ).yzw );
        float3 fluxP = RSMsFlux.SampleLevel( BlackBorderSampler, uv, 0 );

        // Equation 1.
        float3 irrandiance = fluxP * ( ( max( 0.f, dot( normalP, worldPosition - positionP ) )
									* max( 0.f, dot( worldNormal, positionP - worldPosition ) ) )
									/ pow( length( worldPosition - positionP ), 4 ) );
        irrandiance *= samplingOffset.z * samplingOffset.z;
        indirectLight += irrandiance;
    }

    return float4( indirectLight, 1.f );
}