#include "Common/BindlessResources.fxh"
// #include "Common/LightCommon.fxh"
#include "Common/TexCommon.fxh"
#include "Visibility/VisibilityShadingTemplate.fxh"

StructuredBuffer<float3> Normals;
StructuredBuffer<float2> Texcoords;

#if SupportsBindless == 0
Texture2D DiffuseTex;
SamplerState DiffuseTexSampler;
#endif

void ShadingMain( ShadingContext ctx )
{
    BarycentricDeriv deriv = ctx.m_deriv;

    float3 p0 = ctx.m_p0;
    float3 p1 = ctx.m_p1;
    float3 p2 = ctx.m_p2;

    uint3 indices = ctx.m_indices;

    PrimitiveSceneData primitiveData = ctx.m_primitiveData;

    uint2 pixelPosition = ctx.m_pixelPosition;
    float2 screenUV = ctx.m_screenUV;

    float3 position;
    float3 ddx3, ddy3;
    InterpolateFloat3( deriv, p0, p1, p2, position, ddx3, ddy3 );

    float3 n0 = Normals[indices.x];
    float3 n1 = Normals[indices.y];
    float3 n2 = Normals[indices.z];

    float3 normal;
    InterpolateFloat3( deriv, n0, n1, n2, normal, ddx3, ddy3 );

    GeometryProperty geometry = (GeometryProperty)0;
    geometry.worldPos = mul( float4( position, 1.f ), primitiveData.m_worldMatrix ).xyz;
    geometry.viewPos = mul( float4( geometry.worldPos, 1.f ), ViewMatrix ).xyz;
    geometry.normal = mul( float4( normal, 0.f ), transpose( primitiveData.m_invWorldMatrix ) ).xyz;
    geometry.screenUV = ( pixelPosition + 0.5f ) / ScreenSize;

    LIGHTCOLOR cColor = CalcLight( geometry );

    float2 uv0 = Texcoords[indices.x];
    float2 uv1 = Texcoords[indices.y];
    float2 uv2 = Texcoords[indices.z];

    float2 texcoord;
    float2 texcoordDDX;
    float2 texcoordDDY;
    InterpolateFloat2( deriv, uv0, uv1, uv2, texcoord, texcoordDDX, texcoordDDY );

    WriteTextureFeedback( texcoordDDX, texcoordDDY );

#if SupportsBindless == 1
    float4 lightColor = cColor.m_diffuse * MoveLinearSpace( Tex2D[DiffuseTex].SampleGrad( Samplers[DiffuseTexSampler], texcoord, texcoordDDX, texcoordDDY ) );
#else
    float4 lightColor = cColor.m_diffuse * MoveLinearSpace( DiffuseTex.SampleGrad( DiffuseTexSampler, texcoord, texcoordDDX, texcoordDDY ) );
#endif
    lightColor += cColor.m_specular * MoveLinearSpace( Specular );

    SceneColor[pixelPosition] = MoveGammaSpace( float4( lightColor.rgb, 1.f ) );
}