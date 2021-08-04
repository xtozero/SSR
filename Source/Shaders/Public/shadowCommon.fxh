#include "texCommon.fxh"

#define MAX_CASCADED_NUM 2

Texture2DArray shadowTexture : register( t2 );
SamplerState shadowSampler : register( s1 );

struct CascadeConstant
{
	float m_zFar;
	float3 padding;
};

cbuffer SHADOW_CONSTANT : register( b4 )
{
	CascadeConstant g_cascadeConstant[MAX_CASCADED_NUM];
	matrix g_lightViewProjection[MAX_CASCADED_NUM];
};

float SampleShadow( float2 texcoord, int cascadeIndex )
{
	return shadowTexture.Sample( shadowSampler, float3( texcoord, cascadeIndex ) ).r;
}

float PoissonSample4x4Shadow( float2 texcoord, int i, int j, int cascadeIndex )
{
	float2 offset = PoissonSample4x4( i, j ) * 0.00048828125f;

	return shadowTexture.Sample( shadowSampler, float3( texcoord + offset, cascadeIndex ) ).r;
}

float RotatePoissonSample4x4Shadow( float2 texcoord, float2 sin_cos, int i, int j, int cascadeIndex )
{
	float2 offset = RotatePoissonSample4x4( sin_cos.x, sin_cos.y, i, j ) * 0.00048828125f;
	
	return shadowTexture.Sample( shadowSampler, float3( texcoord + offset, cascadeIndex ) ).r;
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

float CalcShadowVisibility( float3 worldPos, float3 viewPos )
{
	int cascadeIndex = SearchCascadeIndex( viewPos.z );
	
	float4 shadowCoord = mul( float4(worldPos, 1.0f), g_lightViewProjection[cascadeIndex] );
	
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
			if ( RotatePoissonSample4x4Shadow( uv_depth.xy, sin_cos, i, j, cascadeIndex ) <= uv_depth.z )
			{
				visibility -= 0.04f;
			}
		}
	}
	
	return visibility;
}