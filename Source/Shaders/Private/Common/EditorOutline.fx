#include "Common/ViewConstant.fxh"
#include "Common/VsCommon.fxh"

/*
* VS_EditorOutline
*/

struct VSInput
{
    float3 position : POSITION;
    uint primitiveId : PRIMITIVEID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VSMain( VSInput input )
{
    VSOutput output = (VSOutput)0;

    PrimitiveSceneData primitiveData = GetPrimitiveData( input.primitiveId );
    float3 worldPosition = mul( float4( input.position, 1.0f ), primitiveData.m_worldMatrix ).xyz;
    float3 viewPosition = mul( float4( worldPosition, 1.0f ), ViewMatrix ).xyz;
    float4 projectionPosition = mul( float4( viewPosition, 1.0f ), ProjectionMatrix );

    output.position = ApplyTAAJittering( projectionPosition );

    return output;
}

/*
* PS_EditorOutline
*/

typedef VSOutput PSInput;

uint PSMain( PSInput input ) : SV_Target0
{
    return 1;
}

/*
* CS_EditorOutline
*/

float3 OutlineColor;
float OutlineWidth;
uint2 ScreenSize;

Texture2D<uint> OutlineMask;
RWTexture2D<float4> SceneColor;

int2 ClampPos( int2 pos )
{
    return clamp( pos, (int2)0, int2( ScreenSize ) - (int2)1 );
}

[numthreads(8, 4, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
    if ( all( DTid.xy < ScreenSize ) )
    {
        int2 centerPos = DTid.xy;
        int2 leftPos = ClampPos( centerPos - int2( OutlineWidth, 0 ) );
        int2 rightPos = ClampPos( centerPos + int2( OutlineWidth, 0 ) );
        int2 topPos = ClampPos( centerPos - int2( 0, OutlineWidth ) );
        int2 bottomPos = ClampPos( centerPos + int2( 0, OutlineWidth ) );
        int2 leftTopPos = ClampPos( centerPos + int2( -OutlineWidth, -OutlineWidth ) * 0.70710f );
        int2 rightTopPos = ClampPos( centerPos + int2( OutlineWidth, -OutlineWidth ) * 0.70710f );
        int2 leftBottomPos = ClampPos( centerPos + int2( -OutlineWidth, OutlineWidth ) * 0.70710f );
        int2 rightBottomPos = ClampPos( centerPos + int2( OutlineWidth, OutlineWidth ) * 0.70710f );

        uint centerMask = OutlineMask[centerPos];
        uint leftMask = OutlineMask[leftPos];
        uint rightMask = OutlineMask[rightPos];
        uint topMask = OutlineMask[topPos];
        uint bottomMask = OutlineMask[bottomPos];
        uint leftTopMask = OutlineMask[leftTopPos];
        uint rightTopMask = OutlineMask[rightTopPos];
        uint leftBottomMask = OutlineMask[leftBottomPos];
        uint rightBottomMask = OutlineMask[rightBottomPos];

        int isOutline = centerMask != leftMask ? 1 : 0;
        isOutline += centerMask != rightMask ? 1 : 0;
        isOutline += centerMask != topMask ? 1 : 0;
        isOutline += centerMask != bottomMask ? 1 : 0;
        isOutline += centerMask != leftTopMask ? 1 : 0;
        isOutline += centerMask != rightTopMask ? 1 : 0;
        isOutline += centerMask != leftBottomMask ? 1 : 0;
        isOutline += centerMask != rightBottomMask ? 1 : 0;

        if ( ( centerMask == 0 ) && ( isOutline > 0 ) )
        {
            SceneColor[centerPos].rgb = OutlineColor;
        }
    }
}