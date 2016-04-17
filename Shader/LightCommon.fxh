#include "texCommon.fxh"

#define MAX_LIGHTS 180

Texture2D lookupTexture : register( t1 );

struct LIGHT_TRAIT
{
	int		m_type;
	bool	m_isOn;
	float	m_theta;
	float	m_phi;
	float3	m_direction;
	float	m_range;
	float	m_falloff;
	float3	m_attenuation;
	float4	m_position;
	float4	m_diffuse;
	float4	m_specular;
};

cbuffer LIGHTS : register( b0 )
{
	int			g_curLights;
	float3		g_cameraPos;
	float4		g_globalAmbient;
	LIGHT_TRAIT	g_lights[MAX_LIGHTS];
};

cbuffer SURFACE : register( b1 )
{
	float4		g_ambient;
	float4		g_diffuse;
	float4		g_specular;
	float		g_roughness;
	float		g_specularPower;
	float		g_pedding[2];
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
};

float4 MoveLinearSpace( float4 color )
{
	return float4( pow( color.xyz, 2.2 ), color.a );
}

float4 MoveGammaSapce( float4 color )
{
	return float4( pow( color.xyz, 0.45 ), color.a );
}

float4 CalcDirectionalLight( int idx, float3 viewDirection, float3 lightDirection, float3 normal, float4 color )
{
	float vdotn = dot( viewDirection, normal );
	float ldotn = dot( lightDirection, normal );
	float gamma = dot ( viewDirection - normal * vdotn, lightDirection - normal * ldotn );

	float roughnessPwr = g_roughness * g_roughness;

	float a = 1 - 0.5f * ( roughnessPwr / ( roughnessPwr + 0.33f ) );
	float b = 0.45f * ( roughnessPwr / ( roughnessPwr + 0.09f ) );

	float2 tc = float2( (vdotn + 1.0f) / 2.0f, (ldotn + 1.0f) / 2.0f );
	float c = lookupTexture.Sample( baseSampler, tc ).r;

	float final = a + b * max( 0.0f, gamma ) * c;

	return max( 0.0f, ldotn ) * final * g_lights[idx].m_diffuse;
}

float4 CalcLight( PS_INPUT input, float4 color )
{
	float4 linearColor = MoveLinearSpace( color );

	float3 viewDirection = normalize( g_cameraPos - input.worldPos );
	float3 normal = normalize( input.normal );

	float4 diffuseFactor = float4( 0.f, 0.f, 0.f, 0.f );

	for ( int i = 0; i < g_curLights; ++i )
	{
		if ( g_lights[i].m_isOn )
		{
			switch( g_lights[i].m_type )
			{
				case 1:
				float3 lightDirection = g_lights[i].m_position.xyz - input.worldPos;
				lightDirection = normalize( lightDirection );
				diffuseFactor += CalcDirectionalLight( i, viewDirection, -g_lights[i].m_direction, normal, linearColor );
				break;
			}
		}
	}

	float4 lightColor = g_globalAmbient * g_ambient;
	lightColor += diffuseFactor * g_diffuse; 

	return saturate( MoveGammaSapce( linearColor * lightColor ) );
}