#include "Common/PrimitiveSceneData.fxh"

static const uint IndirectArgsStride = 12;

static const uint DrawCallIdBits = 16;
static const uint TriangleIndexMask = 0xFFFF0000;
static const uint DrawCallIdMask = 0xFFFF;

static const uint PixelPositionBits = 16;
static const uint PixelPositionMask = 0xFFFF;

uint EncodeVisibility( uint triangleIndex, uint drawcallId )
{
    return ( triangleIndex << DrawCallIdBits ) | drawcallId;
}

void DecodeVisibility( uint visibility, out uint triangleIndex, out uint drawcallId )
{
    triangleIndex = ( visibility & TriangleIndexMask ) >> DrawCallIdBits;
    drawcallId = visibility & DrawCallIdMask;
}

uint EncodePixelPosition( uint2 pixelPosition )
{
    return ( pixelPosition.x << PixelPositionBits ) | ( pixelPosition.y & PixelPositionMask );
}

void DecodePixelPosition( uint value, out uint2 pixelPosition )
{
    pixelPosition.x = ( value >> PixelPositionBits ) & PixelPositionMask;
    pixelPosition.y = ( value & PixelPositionMask );
}

uint GetLowestLane( uint4 mask )
{
    uint4 lowLanes = firstbitlow( mask ) + uint4( 0, 32, 64, 96 );
    return min( min( lowLanes.x, lowLanes.y ), min( lowLanes.z, lowLanes.w ) );
}

uint CountBits( uint4 mask )
{
    uint4 count = countbits( mask );
    return count.x + count.y + count.z + count.w;
}

uint GetWaveLocalIndexMatch( uint4 mask, uint laneIndex )
{
    uint localIndex = 0;
    uint bit = laneIndex;

    [unroll]
    for ( int i = 0; i < 4; ++i )
    {
        uint bits = mask[i];

        if ( bit >= 32 )
        {
            localIndex += countbits( bits );
            bit -= 32;
        }
        else
        {
            localIndex += countbits( bits & ( ( 1u << bit ) - 1 ) );
            break;
        }
    }

    return localIndex;
}

// http://filmicworlds.com/blog/visibility-buffer-rendering-with-material-graphs/
struct BarycentricDeriv
{
    float3 m_lambda;
    float3 m_ddx;
    float3 m_ddy;
};

BarycentricDeriv CalcFullBary( float4 pt0, float4 pt1, float4 pt2, float2 pixelNdc, float2 winSize )
{
    BarycentricDeriv ret = (BarycentricDeriv)0;

    float3 invW = rcp( float3( pt0.w, pt1.w, pt2.w ) );

    float2 ndc0 = pt0.xy * invW.x;
    float2 ndc1 = pt1.xy * invW.y;
    float2 ndc2 = pt2.xy * invW.z;

    float invDet = rcp( determinant( float2x2( ndc2 - ndc1, ndc0 - ndc1) ) );
    ret.m_ddx = float3( ndc1.y - ndc2.y, ndc2.y - ndc0.y, ndc0.y - ndc1.y ) * invDet * invW;
    ret.m_ddy = float3( ndc2.x - ndc1.x, ndc0.x - ndc2.x, ndc1.x - ndc0.x ) * invDet * invW;
    float ddxSum = dot( ret.m_ddx, float3( 1, 1, 1 ) );
    float ddySum = dot( ret.m_ddy, float3( 1, 1, 1 ) );

    float2 deltaVec = pixelNdc - ndc0;
    float interpInvW = invW.x + deltaVec.x * ddxSum + deltaVec.y * ddySum;
    float interpW = rcp( interpInvW );

    ret.m_lambda.x = interpW * ( invW[0] + deltaVec.x * ret.m_ddx.x + deltaVec.y * ret.m_ddy.x );
    ret.m_lambda.y = interpW * ( 0.0f    + deltaVec.x * ret.m_ddx.y + deltaVec.y * ret.m_ddy.y );
    ret.m_lambda.z = interpW * ( 0.0f    + deltaVec.x * ret.m_ddx.z + deltaVec.y * ret.m_ddy.z );

    ret.m_ddx *= ( 2.0f / winSize.x );
    ret.m_ddy *= ( 2.0f / winSize.y );
    ddxSum    *= ( 2.0f / winSize.x );
    ddySum    *= ( 2.0f / winSize.y );

    ret.m_ddy *= -1.0f;
    ddySum    *= -1.0f;

    float interpW_ddx = 1.0f / ( interpInvW + ddxSum );
    float interpW_ddy = 1.0f / ( interpInvW + ddySum );

    ret.m_ddx = interpW_ddx * ( ret.m_lambda * interpInvW + ret.m_ddx ) - ret.m_lambda;
    ret.m_ddy = interpW_ddy * ( ret.m_lambda * interpInvW + ret.m_ddy ) - ret.m_lambda;

    return ret;
}

BarycentricDeriv ComputePerspectiveBarycentricDeriv( float3 p0, float3 p1, float3 p2, PrimitiveSceneData primitiveData, matrix viewProjection, float2 screenUV, float2 screenSize )
{
    matrix localToClip = mul( primitiveData.m_worldMatrix, viewProjection );

    float4 cp0 = mul( float4( p0, 1.f ), localToClip );
    float4 cp1 = mul( float4( p1, 1.f ), localToClip );
    float4 cp2 = mul( float4( p2, 1.f ), localToClip );
    cp0.w = ( cp0.w > 0.f ) ? max( cp0.w, 0.05f ) : min( cp0.w, -0.05f );
    cp1.w = ( cp1.w > 0.f ) ? max( cp1.w, 0.05f ) : min( cp1.w, -0.05f );
    cp2.w = ( cp2.w > 0.f ) ? max( cp2.w, 0.05f ) : min( cp2.w, -0.05f );

    float2 clipSpacePosition = screenUV * float2( 2.f, -2.f ) + float2( -1.f, 1.f );
    return CalcFullBary( cp0, cp1, cp2, clipSpacePosition, screenSize );
}

float3 InterpolateWithDeriv( BarycentricDeriv deriv, float v0, float v1, float v2 )
{
    float3 mergedV = float3( v0, v1, v2 );
    float3 ret;
    ret.x = dot( mergedV, deriv.m_lambda );
    ret.y = dot( mergedV, deriv.m_ddx );
    ret.z = dot( mergedV, deriv.m_ddy );
    return ret;
}

void InterpolateFloat2( BarycentricDeriv deriv, float2 v0, float2 v1, float2 v2, out float2 v, out float2 ddx, out float2 ddy )
{
    float3 x = InterpolateWithDeriv( deriv, v0.x, v1.x, v2.x );
    float3 y = InterpolateWithDeriv( deriv, v0.y, v1.y, v2.y );

    v = float2( x.x, y.x );
    ddx = float2( x.y, y.y );
    ddy = float2( x.z, y.z );
}

void InterpolateFloat3( BarycentricDeriv deriv, float3 v0, float3 v1, float3 v2, out float3 v, out float3 ddx, out float3 ddy )
{
    float3 x = InterpolateWithDeriv( deriv, v0.x, v1.x, v2.x );
    float3 y = InterpolateWithDeriv( deriv, v0.y, v1.y, v2.y );
    float3 z = InterpolateWithDeriv( deriv, v0.z, v1.z, v2.z );

    v = float3( x.x, y.x, z.x );
    ddx = float3( x.y, y.y, z.y );
    ddy = float3( x.z, y.z, z.z );
}