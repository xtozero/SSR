#include "shadowCommon.fxh"

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
	float4 shadowCoord : TEXCOORD1;
};

struct LIGHTCOLOR
{
	float4 m_diffuse;
	float4 m_specular;
};

float4 MoveLinearSpace( float4 color )
{
	return float4( pow( saturate( color.xyz ), 2.2 ), color.a );
}

float4 MoveGammaSapce( float4 color )
{
	return float4( pow( saturate( color.xyz ), 0.45 ), color.a );
}

float OrenNayarDiffuse( float3 viewDirection, float3 lightDirection, float3 normal )
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

	return max( 0.0f, ldotn ) * final;
}

float CookTorranceSpecular( float3 viewDirection, float3 lightDirection, float3 normal )
{
	float3 half = normalize( viewDirection + lightDirection );
	float ndoth = dot( normal, half );
	float ndothPwr = ndoth * ndoth;
	float roughnessPwr = g_roughness * g_roughness;
	float exponent = -( ( 1 - ndothPwr ) / ( roughnessPwr * ndothPwr ) );
	
	float d = exp( exponent ) / ( 4.f * roughnessPwr * ndothPwr * ndothPwr );

	float vdotn = dot( viewDirection, normal );
	float ldotn = dot( lightDirection, normal );
	float vdoth = dot( viewDirection, half );
	float denominator = ( 2.f * ndoth ) / vdoth;
	float ga = vdotn * denominator;
	float gb = ldotn * denominator;
	
	float g = min( ga, min ( gb, 1.f ) );

	float f = lerp( pow( 1.f - vdotn, 5.f ), 1.f, 0.85f );

	return ( d * g * f ) / ( 4 * vdotn * ldotn );
}

LIGHTCOLOR CalcLightProperties( int i, float3 viewDirection, float3 lightDirection, float3 normal )
{
	LIGHTCOLOR lightColor = (LIGHTCOLOR)0;

	float diffuseFactor = OrenNayarDiffuse( viewDirection, lightDirection, normal );
	lightColor.m_diffuse = diffuseFactor * g_lights[i].m_diffuse;
	if ( diffuseFactor > 0.f )
	{
		lightColor.m_specular += CookTorranceSpecular( viewDirection, lightDirection, normal ) * g_lights[i].m_specular;
	}

	return lightColor;
}

float4 CalcLight( PS_INPUT input, float4 color )
{
	int i;
	float4 linearColor = MoveLinearSpace( color );

	float3 viewDirection = normalize( g_cameraPos - input.worldPos );
	float3 normal = normalize( input.normal );

	LIGHTCOLOR LightColor = (LIGHTCOLOR)0;
	LIGHTCOLOR cColor = (LIGHTCOLOR)0;

	for ( i = 0; i < g_curLights; ++i )
	{
		if ( g_lights[i].m_isOn )
		{
			float3 lightDirection = { 0.f, 0.f, 0.f };
			
			if ( length( g_lights[i].m_direction ) > 0.f )
			{
				lightDirection = -normalize( g_lights[i].m_direction );
			}
			else
			{
				lightDirection = normalize( g_lights[i].m_position.xyz - input.worldPos );
			} 

			LightColor = CalcLightProperties( i, viewDirection, lightDirection, normal );
			cColor.m_diffuse += LightColor.m_diffuse;
			cColor.m_specular += LightColor.m_specular;
		}
	}

	float bias = 0.000003;
	float visibility = 1.0f;
	float2 uv = input.shadowCoord.xy / input.shadowCoord.w;
	uv.y = -uv.y;
	uv = uv * 0.5f + 0.5f;

	float curDepth = input.shadowCoord.z / input.shadowCoord.w;

	for ( i = 0; i < 4; ++i )
	{
		if ( ( PoissonSampleShadow( uv, i ) ) < curDepth - bias )
		{
			visibility -= 0.125f;
		}
	}

	float4 lightColor = g_globalAmbient * g_ambient;
	lightColor += cColor.m_diffuse * g_diffuse * visibility;
	lightColor += cColor.m_specular * g_specular * visibility; 

	return saturate( MoveGammaSapce( linearColor * lightColor ) );
}