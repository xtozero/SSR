#include "Common/Constants.fxh"
#include "Common/NormalCompression.fxh"
#include "Common/ViewConstant.fxh"

RaytracingAccelerationStructure AccelerationStructure;

Texture2D ViewSpaceDistance;
Texture2D WorldNormal;

SamplerState BlackBorderSampler;

RWTexture2D<float4> AmbientOcclusion;

uint SampleCount;
float AORadius;
float AOIntensity;

float3 GetWorldPosition( float2 uv, float viewSpaceDistance )
{
    float4 ndcPosition = float4( uv * float2( 2, -2 ) + float2( -1, 1 ), 1.f, 1.f );
    float4 viewRay = mul( ndcPosition, InvProjectionMatrix );
    viewRay /= viewRay.w;

    float3 viewPosition = normalize( viewRay.xyz ) * viewSpaceDistance;
    float4 worldPosition = mul( float4( viewPosition, 1 ), InvViewMatrix );

    return worldPosition.xyz / worldPosition.w;
}

float Noise( float2 seed )
{
	return frac( sin( dot( seed.xy, float2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

// cosine-weighted
float3 SampleHemisphere( float2 xi )
{
    float phi = 2.f * PI * xi.x;
    float cosTheta = sqrt( 1.f - xi.y );
    float sinTheta = sqrt( xi.y );

    return float3( cos( phi ) * sinTheta, sin( phi ) * sinTheta, cosTheta );
}

// https://jcgt.org/published/0006/01/01/paper-lowres.pdf
float3x3 CreateTBN( float3 normal )
{
    float sign = normal.z >= 0.0 ? 1.0 : -1.0;
    float a = -1.0 / ( sign + normal.z );
    float b = normal.x * normal.y * a;

    float3 tangent = float3( 1.0 + sign * normal.x * normal.x * a, sign * b, -sign * normal.x );
    float3 binormal = float3( b, sign + normal.y * normal.y * a, -normal.y );

    return float3x3( tangent, binormal, normal );
}

float3 SampleAODirection( float2 uv, float sampleIndex, float frameIndex, float3x3 tbn )
{
    float temporalOffset = frac( frameIndex * GoldenRatioConjugate );
    float sampleOffset = sampleIndex / SampleCount;

    float2 xi = float2(
        frac( Noise( uv ) + sampleOffset + temporalOffset ),
        frac( Noise( uv + float2( 0.5723f, 0.3891f ) ) + sampleIndex * GoldenRatioConjugate + temporalOffset )
    );

    float3 sampleDir = SampleHemisphere( xi );

    return normalize( mul( sampleDir, tbn ) );
}

struct [raypayload] PayLoad
{
    bool hit : read( caller ) : write( closesthit, miss );
};

[shader("raygeneration")]
void RayGen()
{
    float2 jitter = HALTON_SEQUENCE[FrameCount % MAX_HALTON_SEQUENCE].xy;
    float2 uv = ( DispatchRaysIndex().xy + jitter ) / DispatchRaysDimensions().xy;

    float3 packedNormal = WorldNormal.SampleLevel( BlackBorderSampler, uv, 0 ).yzw;
    float3 worldNormal = SignedOctDecode( packedNormal );
    float3x3 tbn = CreateTBN( worldNormal );

    float viewSpaceDistance = ViewSpaceDistance.SampleLevel( BlackBorderSampler, uv, 0 ).x;
    if ( viewSpaceDistance <= 0.f )
    {
        return;
    }

    float3 worldPosition = GetWorldPosition( uv, viewSpaceDistance );

    float ao = 0.f;

    [loop]
    for ( int i = 0; i < SampleCount; ++i )
    {
        RayDesc ray;
        ray.Origin = worldPosition;
        ray.TMin = 0.001f;
        ray.Direction = SampleAODirection( uv, i, FrameCount, tbn );
        ray.TMax = AORadius;

        PayLoad payload;
        payload.hit = false;
        TraceRay( AccelerationStructure, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xFF, 0, 0, 0, ray, payload );

        ao += payload.hit ? 0.f : 1.f;
    }

    ao /= SampleCount;
    ao = ao / ( ao + (1.0f - ao) * AOIntensity );

    AmbientOcclusion[DispatchRaysIndex().xy] = ao.xxxx;
}

[shader("closesthit")]
void ClosestHit( inout PayLoad payLoad, in BuiltInTriangleIntersectionAttributes attr )
{
    payLoad.hit = true;
}

[shader("miss")]
void Miss( inout PayLoad payLoad )
{
    payLoad.hit = false;
}
