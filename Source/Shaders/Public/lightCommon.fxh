#include "psCommon.fxh"
#include "texCommon.fxh"
#include "ViewConstant.fxh"

#define MAX_LIGHTS 180
static const uint LIGHTDATA_PER_FLOAT4 = 6;

Texture2D lookupTexture : register( t3 );
Buffer<float4> ForwardLight : register( t4 );

struct ForwardLightData
{
	uint	m_type;
	float	m_theta;
	float	m_phi;
	float3	m_direction;
	float	m_range;
	float	m_falloff;
	float3	m_attenuation;
	float3	m_position;
	float4	m_diffuse;
	float4	m_specular;
};

cbuffer ForwardLightConstant : register( b2 )
{
	uint		NumLights;
	float3		CameraPos;
};

cbuffer Material : register( b3 )
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float		Roughness;
	float		SpecularPower;
	float		pedding[2];
};

struct GeometryProperty
{
	float3 worldPos;
	float3 viewPos;
	float3 normal;
};

struct LIGHTCOLOR
{
	float4 m_diffuse;
	float4 m_specular;
};

ForwardLightData GetLight( uint index )
{
	uint base = index * LIGHTDATA_PER_FLOAT4;
	
	float4 positionAndRange = ForwardLight[base + 0];
	float4 diffuse = ForwardLight[base + 1];
	float4 specular = ForwardLight[base + 2];
	float4 attenuationAndFalloff = ForwardLight[base + 3];
	float4 directionAndType = ForwardLight[base + 4];
	float4 spotAngles = ForwardLight[base + 5];
	
	ForwardLightData light;

	light.m_type = directionAndType.w;
	light.m_theta = spotAngles.x;
	light.m_phi = spotAngles.y;
	light.m_direction = directionAndType.xyz;
	light.m_range = positionAndRange.w;
	light.m_falloff = attenuationAndFalloff.w;
	light.m_attenuation = attenuationAndFalloff.xyz;
	light.m_position = positionAndRange.xyz;
	light.m_diffuse = diffuse;
	light.m_specular = specular;
	
	return light;
}

float4 MoveLinearSpace( float4 color )
{
	return float4( pow( saturate( color.xyz ), 2.2 ), color.a );
}

float4 MoveGammaSapce( float4 color )
{
	return float4( pow( saturate( color.xyz ), 0.45 ), color.a );
}

float OrenNayarDiffuse( float3 viewDirection, float3 lightDirection, float3 normal, float roughness )
{
	float vdotn = dot( viewDirection, normal );
	float ldotn = dot( lightDirection, normal );

	float gamma = dot ( viewDirection - normal * vdotn, lightDirection - normal * ldotn );

	float roughnessPwr = roughness * roughness;

	float a = 1 - 0.5f * ( roughnessPwr / ( roughnessPwr + 0.33f ) );
	float b = 0.45f * ( roughnessPwr / ( roughnessPwr + 0.09f ) );

	float2 tc = float2( (vdotn + 1.0f) / 2.0f, (ldotn + 1.0f) / 2.0f );
	float c = lookupTexture.Sample( baseSampler, tc ).r;

	float final = a + b * max( 0.0f, gamma ) * c;

	return max( 0.0f, ldotn ) * final;
}

float CookTorranceSpecular( float3 viewDirection, float3 lightDirection, float3 normal, float roughness )
{
	float3 half = normalize( viewDirection + lightDirection );
	float ndoth = dot( normal, half );
	float ndothPwr = ndoth * ndoth;
	float roughnessPwr = roughness * roughness;
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

LIGHTCOLOR CalcLightProperties( ForwardLightData light, float3 viewDirection, float3 lightDirection, float3 normal, float roughness )
{
	LIGHTCOLOR lightColor = (LIGHTCOLOR)0;

	// ToDo 
	// float diffuseFactor = OrenNayarDiffuse( viewDirection, lightDirection, normal, roughness );
	float ndotl = saturate( dot( lightDirection, normal ) );
	lightColor.m_diffuse = light.m_diffuse * ndotl;
	if ( ndotl > 0.f )
	{
		lightColor.m_specular += CookTorranceSpecular( viewDirection, lightDirection, normal, roughness ) * light.m_specular * ndotl;
	}

	return lightColor;
}

float3 HemisphereLight( float3 normal )
{
	float w = ( dot( normal, HemisphereUpVector.xyz ) + 1 ) * 0.5;
	return lerp( HemisphereLowerColor, HemisphereUpperColor, w ).xyz;
}

float4 CalcLight( GeometryProperty geometry )
{
	float3 viewDirection = normalize( CameraPos - geometry.worldPos );
	float3 normal = normalize( geometry.normal );

	LIGHTCOLOR LightColor = (LIGHTCOLOR)0;
	LIGHTCOLOR cColor = (LIGHTCOLOR)0;

	float roughness = saturate( Roughness - EPSILON) + EPSILON;

	for ( uint i = 0; i < NumLights; ++i )
	{
		ForwardLightData light = GetLight( i );
	
		float3 lightDirection = { 0.f, 0.f, 0.f };
			
		if ( length( light.m_direction ) > 0.f )
		{
			lightDirection = -normalize( light.m_direction );
		}	
		else
		{
			lightDirection = normalize( light.m_position - geometry.worldPos );
		} 

		LightColor = CalcLightProperties( light, viewDirection, lightDirection, normal, roughness );
		cColor.m_diffuse += LightColor.m_diffuse;
		cColor.m_specular += LightColor.m_specular;
	}

	// ToDo
	float visibility = 1.f; // CalcShadowVisibility( geometry.worldPos, geometry.viewPos );

	float4 lightColor = float4( HemisphereLight( normal ), 1 ) * MoveLinearSpace( Diffuse );
	lightColor += cColor.m_diffuse * MoveLinearSpace( Diffuse ) * visibility;
	lightColor += cColor.m_specular * MoveLinearSpace( Specular ) * visibility; 

	return saturate( MoveGammaSapce( lightColor ) );
}