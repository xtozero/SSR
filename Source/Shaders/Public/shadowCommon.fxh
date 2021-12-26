#include "ShadowConstant.fxh"
#include "texCommon.fxh"

Texture2DArray ShadowTexture : register( t2 );
SamplerState ShadowSampler : register( s2 );

float SampleShadow( float3 uv_depth, int cascadeIndex )
{
	float2 samplePos = uv_depth.xy;
	
	return ShadowTexture.Sample( ShadowSampler, float3( samplePos, cascadeIndex ) );
}

float PoissonDiskSampleShadow( float3 uv_depth, uint i, int cascadeIndex )
{
	float2 offset = PoissonDiskSample( i ) * 0.00048828125f;
	float2 samplePos = uv_depth.xy + offset;

	return ShadowTexture.Sample( ShadowSampler, float3( samplePos, cascadeIndex ) );
}


float RotatePoissonDiskSampleShadow( float3 uv_depth, float2 sin_cos, uint i, int cascadeIndex )
{
	float2 offset = RotatePoissonDiskSample( sin_cos.x, sin_cos.y, i ) * 0.00048828125f;
	float2 samplePos = uv_depth.xy + offset;
	
	return ShadowTexture.Sample( ShadowSampler, float3( samplePos, cascadeIndex ) );
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
	for ( uint i = 0; i < 64; ++i )
	{
		if ( RotatePoissonDiskSampleShadow( uv_depth, sin_cos, i, cascadeIndex ) < uv_depth.z )
		{
			visibility -= ( 0.5f / 64.f );
		}
	}

	return visibility;
}