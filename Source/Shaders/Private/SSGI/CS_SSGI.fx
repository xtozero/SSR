#include "Common/GammaCorrection.fxh"

#include "Common/Constants.h"
#include "Common/NormalCompression.fxh"
#include "Common/ViewConstant.fxh"

Texture2D SceneColor : register( t0 );
Texture2D ViewSpaceDistance : register( t1 );
Texture2D WorldNormal : register( t2 );

SamplerState BlackBorderSampler : register( s0 );

RWTexture2D<float4> SSGI : register( u0 );

float Thickness;
float ViewSpaceRadius;
uint NumSlices;
uint NumSteps;
float2 ScreenSize;
float2 InvScreenSize;
float ColorIntensity;

static const uint BitmaskSize = 32;

static const uint HilbertLevel = 6;
static const uint HilbertWidth = ( (1U << HilbertLevel) );

// code from https://github.com/GameTechDev/XeGTAO
uint HilbertIndex( uint posX, uint posY ) 
{
    uint index = 0;
    
    for ( uint curLevel = HilbertWidth / 2; curLevel > 0; curLevel /= 2 ) 
    {
        uint regionX = ( posX & curLevel ) > 0;
        uint regionY = ( posY & curLevel ) > 0;
        index += curLevel * curLevel * ( ( 3 * regionX ) ^ regionY );
        if ( regionY == 0 ) 
        {
            if ( regionX == 1 ) 
            {
                posX = uint( ( HilbertWidth - 1 ) ) - posX;
                posY = uint( ( HilbertWidth - 1 ) ) - posY;
            }
            
            uint temp = posX;
            posX = posY;
            posY = temp;
        }
    }
    
    return index;
}

// code from https://github.com/GameTechDev/XeGTAO
float2 SpatioTemporalNoise( uint2 pixPos, uint temporalIndex )
{
	uint index = HilbertIndex( pixPos.x, pixPos.y );
    index += 288 * ( temporalIndex % 64 );
    return float2( frac( 0.5 + index * float2( 0.75487766624669276005, 0.5698402909980532659114 ) ) );
}

/*
uint Hash32( uint x )
{
	x ^= x >> 16;
	x *= uint( 0x7feb352d );
	x ^= x >> 15;
	x *= uint( 0x846ca68b );
	x ^= x >> 16;
	return x;
}

float Hash32ToFloat( uint hash )
{ 
	return hash / 4294967296.0;
}

uint Hash32Combine( const uint seed, const uint value )
{
	return seed ^ ( Hash32( value ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 ) );
}

float2 SpatioTemporalNoise( uint2 pixPos, uint temporalIndex )
{
	uint baseHash = Hash32( pixPos.x + ( pixPos.y << 15 ) );
	baseHash = Hash32Combine( baseHash, temporalIndex );
	return float2( Hash32ToFloat( baseHash ), Hash32ToFloat( Hash32( baseHash ) ) );
}
*/

float3 GetViewPosition( float2 uv, float viewSpaceDistance )
{
    float4 ndcPosition = float4( uv * float2( 2, -2 ) + float2( -1, 1 ), 1.f, 1.f );
    float4 viewRay = mul( ndcPosition, InvProjectionMatrix );
    viewRay /= viewRay.w;

    return normalize( viewRay.xyz ) * viewSpaceDistance;
}

float3 GetViewNormal( float2 uv )
{
    float3 packedNormal = WorldNormal.SampleLevel( BlackBorderSampler, uv, 0 ).yzw;
    float3 worldNormal = SignedOctDecode( packedNormal );
    float3 viewNormal = mul( worldNormal, (float3x3)ViewMatrix );

    return normalize( viewNormal );
}

float DetermineStepSizeInPixel( float radiusInPixel )
{
    float numSteps = min( NumSteps, radiusInPixel );
    float stepSizeInPixel = radiusInPixel / ( numSteps + 1.f );

    return stepSizeInPixel;
}

float3 GenerateGI( float2 uv, float2 deltaUV, float3 p, float3 np, uint numSteps, float stepOffset )
{
    const float invHalfPI = 1.f / ( 0.5f * PI );

    float2 sampleUV = uv + stepOffset * deltaUV;

    float3 viewVec = normalize( p );
    float3 deltaThickness = viewVec * Thickness;

    float3 normal = -viewVec;

    float3 gi = (float3)0.f;
    uint bi = 0;

    [loop]
    for ( uint step = 0; step < numSteps; ++step )
    {
        if ( any( sampleUV < 0.f ) || any( sampleUV > 1.f ) )
        {
            break;
        }

        float viewSpaceDistance = ViewSpaceDistance.SampleLevel( BlackBorderSampler, sampleUV, 0 ).x;
        float3 sf = GetViewPosition( sampleUV, viewSpaceDistance );
        float3 sb = sf + deltaThickness;

        float tf = acos( saturate( dot( normal, sf - p ) ) );
        float tb = acos( saturate( dot( normal, sb - p ) ) );

        float tmin = min( tf, tb );
        float tmax = max( tf, tb );

        uint a = floor( tmin * invHalfPI * BitmaskSize );
        uint b = floor( ( tmax - tmin ) * invHalfPI * BitmaskSize );

        uint bj = ( ( 1 << b ) - 1 ) << a;

        float3 cj = MoveLinearSpace( SceneColor.SampleLevel( BlackBorderSampler, sampleUV, 0 ) ).rgb;
        float3 nj = GetViewNormal( sampleUV );
        float3 lj = normalize( sf - p );

        gi += float( countbits( bj & ( ~bi ) ) ) / BitmaskSize * cj * saturate( dot( np, lj ) ) * saturate( dot( nj, -lj ) );

        bi |= bj;

        sampleUV += deltaUV;
    }

    return gi;
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 gi = (float3)0.f;

    float2 uv = ( DTid.xy + 0.5f ) * InvScreenSize;

    float2 noise = SpatioTemporalNoise( DTid.xy, FrameCount );

    [branch]
    if ( all( uv < 1.f ) )
    {
        float viewSpaceDistance = ViewSpaceDistance.SampleLevel( BlackBorderSampler, uv, 0 ).x;
        float3 viewPosition = GetViewPosition( uv, viewSpaceDistance );
        float3 viewNormal = GetViewNormal( uv );

        float halfWidth = ScreenSize.x * 0.5f;
        float radiusInPixel = ViewSpaceRadius * ProjectionMatrix[0][0] * halfWidth / viewPosition.z;

        [branch]
        if ( radiusInPixel > 1.f )
        {
            float stepSizeInPixel = DetermineStepSizeInPixel( radiusInPixel );
            uint numSteps = radiusInPixel / stepSizeInPixel;

            float2 stepSizeInUV = stepSizeInPixel * InvScreenSize;

            [loop]
            for ( int slice = 0; slice < NumSlices; ++slice )
            {
                float t = ( float( slice ) + noise.x ) / float( NumSlices );
                float theta = 2.f * PI * t;

                float2 dir = float2( cos( theta ), sin( theta ) );
                float2 deltaUV = stepSizeInUV * normalize( dir );

                gi += GenerateGI( uv, deltaUV, viewPosition, viewNormal, numSteps, noise.y );
            }
        }

        gi *= ColorIntensity;

        SSGI[DTid.xy] = MoveGammaSpace( float4( gi / NumSlices, 1.f ) );
    }
}