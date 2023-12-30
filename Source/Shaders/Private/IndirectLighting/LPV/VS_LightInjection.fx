#include "Common/NormalCompression.fxh"
#include "Common/ViewConstant.fxh"
#include "IndirectLighting/LPV/LPVCommon.fxh"
#include "Shadow/ShadowCommon.fxh"

Texture2DArray RSMsWorldPosition : register( t0 );
Texture2DArray RSMsNormal : register( t1 );
Texture2DArray RSMsFlux : register( t2 );

cbuffer LightInjectionParameters : register( b0 )
{
    uint3 RSMsDimensions;
    float4 SurfelArea[MAX_CASCADED_NUM];
}

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
    float3 flux : FLUX;
    int skip : SKIP;
    float surfelArea : SURFEL_AREA;
};

VS_OUTPUT main( uint vertexId : SV_VertexID )
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    uint zIndex = vertexId / ( RSMsDimensions.x * RSMsDimensions.y );
    uint sliceIndex = vertexId % ( RSMsDimensions.x * RSMsDimensions.y );
    uint xIndex = sliceIndex % RSMsDimensions.x;
    uint yIndex = sliceIndex / RSMsDimensions.x;

    uint4 uv = { xIndex, yIndex, zIndex, 0 };
    float3 worldPosition = RSMsWorldPosition.Load( uv ).xyz;

    float3 packedNormal = RSMsNormal.Load( uv ).yzw;
    float3 normal = SignedOctDecode( packedNormal );

    int3 gridPos = GetGridPos( worldPosition, normal );

    float2 screenPosition = { ( gridPos.x + 0.5f ) / TexDimension.x, ( gridPos.y + 0.5f ) / TexDimension.y };
    screenPosition = screenPosition * 2.f - 1.f;
    screenPosition.y = -screenPosition.y;

    output.position = float4( screenPosition, gridPos.z, 1.f );
    output.normal = normal;
    output.flux = RSMsFlux.Load( uv ).rgb;

    float3 viewPosition = mul( float4( worldPosition, 1.f ), ViewMatrix ).xyz;
    int cascadeIndex = SearchCascadeIndex( viewPosition.z );
    output.surfelArea = SurfelArea[cascadeIndex].x;
    output.skip = ( zIndex == cascadeIndex ) ? 0 : 1;

    return output;
}