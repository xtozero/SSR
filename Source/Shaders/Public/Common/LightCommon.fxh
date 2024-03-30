#include "Common/GammaCorrection.fxh"
#include "Common/PsCommon.fxh"
#include "Common/TexCommon.fxh"
#include "Common/ViewConstant.fxh"

#define MAX_LIGHTS 180
static const uint LIGHTDATA_PER_FLOAT4 = 6;
static const uint LightTypeDirectional = 0;
static const uint LightTypePoint = 1;
static const uint LightTypeSpot = 2;

Texture2D lookupTexture : register( t3 );
Buffer<float4> ForwardLight : register( t4 );

Texture2D IndirectIllumination : register( t5 );
SamplerState LinearSampler : register( s1 );

TextureCube IrradianceMap : register( t6 );

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
	uint NumLights : packoffset( c0.x );
	float3 HemisphereUpVector : packoffset( c0.y );
	float4 HemisphereUpperColor : packoffset( c1 );
	float4 HemisphereLowerColor : packoffset( c2 );
	float4 IrradianceMapSH[7] : packoffset( c3 );
};

cbuffer Material : register( b3 )
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float		Roughness;
	float		SpecularExponent;
	float		Metallic;
};

struct GeometryProperty
{
	float3 worldPos;
	float3 viewPos;
	float3 normal;
	float2 screenUV;
};

struct SurfaceProperty
{
	float3 albedo;
	float roughness;
	float metallic;
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

float3 FresnelSchlick( float cosTheta, float3 f0 )
{
	return f0 + ( 1.f - f0 ) * pow( saturate( 1.f - cosTheta ), 5.f );
}

float DistributionGGX( float ndoth, float roughness )
{
	float a = roughness * roughness;
	float a2 = a * a;
	float ndoth2 = ndoth * ndoth;

	float num = a2;
	float denom = ( ndoth2 * ( a2 - 1.f ) + 1.f );
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX( float ndotv, float roughness )
{
	float r = roughness + 1.f;
	float k = ( r * r ) / 8.f;

	float num = ndotv;
	float denom = ndotv * ( 1.f - k ) + k;

	return num / denom;
}

float GemoetrySmith( float ndotl, float ndotv, float roughness )
{
	float ggx1 = GeometrySchlickGGX( ndotl, roughness );
	float ggx2 = GeometrySchlickGGX( ndotv, roughness );

	return ggx1 * ggx2;
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

LIGHTCOLOR CalcLightProperties( ForwardLightData light, float3 worldPosition, float3 viewDirection, float3 normal, float roughness )
{
	LIGHTCOLOR lightColor = (LIGHTCOLOR)0;

    float3 toLight = normalize( light.m_position - worldPosition );
    if ( light.m_type == LightTypeDirectional )
    {
        toLight = -light.m_direction;
    }
    else if ( light.m_type == LightTypePoint )
    {
        light.m_direction = -toLight;
    }
	
    float ndotl = saturate( dot( toLight, normal ) );
	lightColor.m_diffuse = light.m_diffuse * ndotl;
    lightColor.m_specular = CookTorranceSpecular( viewDirection, toLight, normal, roughness ) * light.m_specular * ndotl;
	
    float distance = length( light.m_position - worldPosition );
    float attenuation = 1.f / ( light.m_attenuation.x + light.m_attenuation.y * distance + light.m_attenuation.z * distance * distance );

    float theta = dot( toLight, -light.m_direction );
    float epsilon = ( light.m_theta - light.m_phi );
    float intensity = saturate( ( theta - light.m_phi ) / epsilon );
	
    lightColor.m_diffuse *= attenuation * intensity;
    lightColor.m_specular *= attenuation * intensity;
	
	return lightColor;
}

LIGHTCOLOR CalcPhysicallyBasedLightProperties( ForwardLightData light, float3 worldPosition, float3 viewDirection, float3 normal, SurfaceProperty surface )
{
	LIGHTCOLOR lightColor = (LIGHTCOLOR)0;

    float3 toLight = normalize( light.m_position - worldPosition );
    if ( light.m_type == LightTypeDirectional )
    {
        toLight = -light.m_direction;
    }
    else if ( light.m_type == LightTypePoint )
    {
        light.m_direction = -toLight;
    }

	float3 f0 = (float3)0.04f;
	f0 = lerp( f0, surface.albedo, surface.metallic );
	
	float3 halfway = normalize( viewDirection + toLight );
    float ndotl = saturate( dot( toLight, normal ) );
	float hdotv = saturate( dot( halfway, viewDirection ) );
	float ndotv = saturate( dot( normal, viewDirection ) );
	float ndoth = saturate( dot( normal, halfway ) );

	float N = DistributionGGX( ndoth, surface.roughness );
	float G = GemoetrySmith( ndotl, ndotv, surface.roughness );
	float3 F = FresnelSchlick( hdotv, f0 );

	float3 kS = F;
	float3 kD = (float3)1.f - kS;
	kD *= 1.f - surface.metallic;
	
	lightColor.m_diffuse = float4( kD / PI, 1.f ) * light.m_diffuse * ndotl;

	float3 numerator = N * G * F;
	float denominator = 4.f * ndotv * ndotl + 0.0001f;

    lightColor.m_specular = float4( numerator / denominator, 1.f ) * light.m_specular * ndotl;
	
    float distance = length( light.m_position - worldPosition );
    float attenuation = 1.f / ( light.m_attenuation.x + light.m_attenuation.y * distance + light.m_attenuation.z * distance * distance );

    float theta = dot( toLight, -light.m_direction );
    float epsilon = ( light.m_theta - light.m_phi );
    float intensity = saturate( ( theta - light.m_phi ) / epsilon );
	
    lightColor.m_diffuse *= attenuation * intensity;
    lightColor.m_specular *= attenuation * intensity;
	
	return lightColor;
}

float3 HemisphereLight( float3 normal )
{
	float w = ( dot( normal, HemisphereUpVector ) + 1 ) * 0.5;
	return lerp( HemisphereLowerColor, HemisphereUpperColor, w ).rgb;
}

float3 ImageBasedLight( float3 normal )
{
#if UseIrradianceMapSH == 1
	float3 l00 = { IrradianceMapSH[0].x, IrradianceMapSH[0].y, IrradianceMapSH[0].z }; // L00
	float3 l1_1 = { IrradianceMapSH[0].w, IrradianceMapSH[1].x, IrradianceMapSH[1].y }; // L1-1
	float3 l10 = { IrradianceMapSH[1].z, IrradianceMapSH[1].w, IrradianceMapSH[2].x }; // L10
	float3 l11 = { IrradianceMapSH[2].y, IrradianceMapSH[2].z, IrradianceMapSH[2].w }; // L11
	float3 l2_2 = { IrradianceMapSH[3].x, IrradianceMapSH[3].y, IrradianceMapSH[3].z }; // L2-2
	float3 l2_1 = { IrradianceMapSH[3].w, IrradianceMapSH[4].x, IrradianceMapSH[4].y }; // L2-1
	float3 l20 = { IrradianceMapSH[4].z, IrradianceMapSH[4].w, IrradianceMapSH[5].x }; // L20
	float3 l21 = { IrradianceMapSH[5].y, IrradianceMapSH[5].z, IrradianceMapSH[5].w }; // L21
	float3 l22 = { IrradianceMapSH[6].x, IrradianceMapSH[6].y, IrradianceMapSH[6].z }; // L22

	static const float c1 = 0.429043f;
	static const float c2 = 0.511664f;
	static const float c3 = 0.743125f;
	static const float c4 = 0.886227f;
	static const float c5 = 0.247708f;

	return c1 * l22 * ( normal.x * normal.x - normal.y * normal.y ) + c3 * l20 * normal.z * normal.z + c4 * l00 - c5 * l20
		+ 2.f * c1 * ( l2_2 * normal.x * normal.y + l21 * normal.x * normal.z + l2_1 * normal.y * normal.z )
		+ 2.f * c2 * ( l11 * normal.x + l1_1 * normal.y + l10 * normal.z );
#else
    return IrradianceMap.Sample( LinearSampler, normal ).rgb;
#endif
}

LIGHTCOLOR CalcLight( GeometryProperty geometry )
{
	float3 viewDirection = normalize( CameraPos - geometry.worldPos );
	float3 normal = normalize( geometry.normal );

	LIGHTCOLOR LightColor = (LIGHTCOLOR)0;
	LIGHTCOLOR cColor = (LIGHTCOLOR)0;

	float roughness = saturate( Roughness - EPSILON) + EPSILON;

	for ( uint i = 0; i < NumLights; ++i )
	{
		ForwardLightData light = GetLight( i );
        LightColor = CalcLightProperties( light, geometry.worldPos, viewDirection, normal, roughness );

		// ToDo
		float visibility = 1.f; // CalcShadowVisibility( geometry.worldPos, geometry.viewPos );

		cColor.m_diffuse += LightColor.m_diffuse * visibility;
		cColor.m_specular += LightColor.m_specular * visibility;
	}

	cColor.m_diffuse.rgb += ImageBasedLight( normal );
	cColor.m_diffuse.rgb += HemisphereLight( normal );

#if EnableRSMs == 1
	cColor.m_diffuse.rgb += IndirectIllumination.Sample( LinearSampler, geometry.screenUV ).rgb;
#endif

	return cColor;
}

LIGHTCOLOR CalcPhysicallyBasedLight( GeometryProperty geometry, SurfaceProperty surface )
{
	float3 viewDirection = normalize( CameraPos - geometry.worldPos );
	float3 normal = normalize( geometry.normal );

	LIGHTCOLOR LightColor = (LIGHTCOLOR)0;
	LIGHTCOLOR cColor = (LIGHTCOLOR)0;

	for ( uint i = 0; i < NumLights; ++i )
	{
		ForwardLightData light = GetLight( i );
        LightColor = CalcPhysicallyBasedLightProperties( light, geometry.worldPos, viewDirection, normal, surface );

		// ToDo
		float visibility = 1.f; // CalcShadowVisibility( geometry.worldPos, geometry.viewPos );

		cColor.m_diffuse += LightColor.m_diffuse * visibility;
		cColor.m_specular += LightColor.m_specular * visibility;
	}

	cColor.m_diffuse.rgb += ImageBasedLight( normal );
	cColor.m_diffuse.rgb += HemisphereLight( normal );

#if EnableRSMs == 1
	cColor.m_diffuse.rgb += IndirectIllumination.Sample( LinearSampler, geometry.screenUV ).rgb;
#endif

	return cColor;
}
