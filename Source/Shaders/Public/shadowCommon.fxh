#include "ShadowConstant.fxh"
#include "texCommon.fxh"

Texture2DArray ShadowTexture : register( t2 );
SamplerState ShadowSampler : register( s2 );

float SampleShadow( float2 texcoord, int cascadeIndex )
{
	return ShadowTexture.Sample( ShadowSampler, float3( texcoord, cascadeIndex ) ).r;
}

float PoissonSample4x4Shadow( float2 texcoord, int i, int j, int cascadeIndex )
{
	float2 offset = PoissonSample4x4( i, j ) * 0.00048828125f;

	return ShadowTexture.Sample( ShadowSampler, float3( texcoord + offset, cascadeIndex ) ).r;
}

float RotatePoissonSample4x4Shadow( float2 texcoord, float2 sin_cos, int i, int j, int cascadeIndex )
{
	float2 offset = RotatePoissonSample4x4( sin_cos.x, sin_cos.y, i, j ) * 0.00048828125f;
	
	return ShadowTexture.Sample( ShadowSampler, float3( texcoord + offset, cascadeIndex ) ).r;
}

int SearchCascadeIndex( float dist )
{
	int index = MAX_CASCADED_NUM - 1;
	for ( int i = 0; i < MAX_CASCADED_NUM; ++i )
	{
		if ( dist <= g_cascadeConstant[i].m_zFar )
		{
			index = i;
			break;
		}
	}

	return index;
}

float3 GetNormalOffset( float nDotL, float3 normal )
{
	/*
	float2 shadowTextureSize;
	float elements;
	
	ShadowTexture.GetDimensions( shadowTextureSize.x, shadowTextureSize.y, elements );
	float texelSize = 2.0f / shadowTextureSize.x;
	*/

	float normalOffsetScale = saturate( 1.0f - nDotL );

	// return normal *( g_constantBias + g_slopeBiasScale * texelSize * normalOffsetScale );
	return normal *( g_constantBias + g_slopeBiasScale * normalOffsetScale );
}

float CalcShadowVisibility( float3 worldPos, float3 worldNormal, float3 viewPos )
{
	int cascadeIndex = SearchCascadeIndex( viewPos.z );
	
	float nDotL = saturate( dot( -g_lightPosOrDir, worldNormal ) );
	float3 offset = GetNormalOffset( nDotL, worldNormal );
	
	float4 shadowCoord = mul( float4(worldPos + offset, 1.0f), g_shadowViewProjection[cascadeIndex] );
	
	float3 uv_depth = shadowCoord.xyz / shadowCoord.w;
	uv_depth.y = -uv_depth.y;
	uv_depth.xy = uv_depth.xy * 0.5f + 0.5f;

	float angle = random( worldPos ) % 360.f;
	float2 sin_cos = float2( sin(angle), cos(angle) );

	float visibility = 1.0f;
	for ( int i = 0; i < 4; ++i )
	{
		for ( int j = 0; j < 4; ++j )
		{
			if ( RotatePoissonSample4x4Shadow( uv_depth.xy, sin_cos, i, j, cascadeIndex ) < uv_depth.z )
			{
				visibility -= 0.03f;
			}
		}
	}
	
	return visibility;
}