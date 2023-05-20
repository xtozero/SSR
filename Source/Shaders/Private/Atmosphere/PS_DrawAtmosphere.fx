#define ATMOSPHERE_RENDERING

#include "Atmosphere/AtmosphereCommon.fxh"

Texture2D ViewSpaceDistance : register( t3 );
SamplerState ViewSpaceDistanceSampler : register( s3 );

static const float ISun = 100.f;

cbuffer SkyAtmosphereRenderParameter : register( b1 )
{
	float4 CameraPos : packoffset( c0 );
	float4 SunDir : packoffset( c1 );
	float Exposure : packoffset( c2 );
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION0;
	float3 viewRay : POSITION1;
	float3 worldRay : POSITION2;
	float2 uv : TEXCOORD0;
};

float3 InscatterColor( float3 x, float t, float3 viewRay, float r, float mu, out float3 attenuation )
{
	float3 result = 0.f;
	attenuation = 1.f;

	float d = -r * mu - sqrt( r * r * (mu * mu - 1.f) + Rt * Rt );
	if ( d > 0.f )
	{
		x += d * viewRay;
		t -= d;
		mu = (r * mu + d) / Rt;
		r = Rt;
	}

	static const float EPS = 0.005f;
	/*
	if ( r < Rg + HeightOffset + EPS )
	{
		float diff = ( Rg + HeightOffset + EPS ) - r;
		x -= diff * viewRay;
		t -= diff;
		mu = dot( x, viewRay ) / r;
		r = Rg + HeightOffset + EPS;
	}
	*/

	if ( r < Rt )
	{
		float nu = dot( viewRay, SunDir.xyz );
		float muS = dot( x, SunDir.xyz ) / r;

		float4 inscatter = max( Sample4D( InscatterLut, InscatterLutSampler, r, mu, muS, nu ), 0.f );

		float mu0 = mu;
		float muS0 = muS;
		if ( t > 0.f )
		{
			float3 x0 = x + t * viewRay;
			float altitude0 = length( x0 );
			mu0 = dot( x0, viewRay ) / altitude0;
			muS0 = dot( x0, SunDir.xyz ) / altitude0;

			attenuation = AnalyticTransmittance( r, mu, t );
			inscatter = max( inscatter - attenuation.rgbr * Sample4D( InscatterLut, InscatterLutSampler, altitude0, mu0, muS0, nu ), 0.f );
		}

		float muHoriz = -sqrt( 1.f - (Rg / r) * (Rg / r) );
		if ( abs( mu - muHoriz ) < EPS )
		{
			float a = ((mu - muHoriz) + EPS) / (2.f * EPS);

			mu = muHoriz - EPS;
			float altitude0 = sqrt( r * r + t * t + 2.f * r * t * mu );
			mu0 = (r * mu + t) / altitude0;

			float4 inscatter0 = Sample4D( InscatterLut, InscatterLutSampler, r, mu, muS, nu );
			float4 inscatter1 = Sample4D( InscatterLut, InscatterLutSampler, altitude0, mu0, muS0, nu );
			float4 inscatterA = max( inscatter0 - attenuation.rgbr * inscatter1, 0.f );

			mu = muHoriz + EPS;
			altitude0 = sqrt( r * r + t * t + 2.f * r * t * mu );
			mu0 = (r * mu + t) / altitude0;

			inscatter0 = Sample4D( InscatterLut, InscatterLutSampler, r, mu, muS, nu );
			inscatter1 = Sample4D( InscatterLut, InscatterLutSampler, altitude0, mu0, muS0, nu );
			float4 inscatterB = max( inscatter0 - attenuation.rgbr * inscatter1, 0.f );

			inscatter = lerp( inscatterA, inscatterB, a );
		}

		inscatter.w *= smoothstep( 0.f, 0.02f, muS );

		float phaseR = PhaseFunctionR( nu );
		float phaseM = PhaseFunctionM( nu );
		result = max( inscatter.rgb * phaseR + getMie( inscatter ) * phaseM, 0.f );
	}

	return result * ISun;
}

float3 GroundColor( float3 x, float t, float3 viewRay, float r, float mu, float3 attenuation, bool isSceneGeometry )
{
	float3 result = 0.f;

	if ( t > 0.f )
	{
		float3 x0 = x + t * viewRay;
		float altitude0 = length( x0 );
		float3 n = x0 / altitude0;

		float muS = dot( n, SunDir.xyz );
		float3 sunLight = isSceneGeometry ? 0 : TransmittanceWithShadow( altitude0, muS );

		float3 groundSkyLight = Irradiance( altitude0, muS );

		float3 sceneColor = isSceneGeometry ? 0 : float3( 0.35f, 0.35f, 0.35f );
		float3 reflectance = ( altitude0 > Rg + HeightOffset ) ? 0.4f : 0.2f;
		reflectance *= sceneColor;

		float3 groundColor = reflectance * ISun / M_PI * ( max( muS, 0.f ) ) * ( sunLight + groundSkyLight );

		result = attenuation * groundColor;
	}

	return result;
}

float3 SunColor( float3 x, float t, float3 viewRay, float r, float mu, bool isSceneGeometry )
{
	if ( isSceneGeometry || t > 0.f )
	{
		return 0.f;
	}
	else
	{
		float3 transmittance = r <= Rt ? TransmittanceWithShadow( r, mu ) : 1.f;
		float isun = step( cos( M_PI / 180 ), dot( viewRay, SunDir.xyz ) ) * ISun;
		return transmittance * isun;
	}
}

float3 HDR( float3 l )
{
	l = l * Exposure;
	l.r = l.r < 1.413f ? pow( abs( l.r * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.r );
	l.g = l.g < 1.413f ? pow( abs( l.g * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.g );
	l.b = l.b < 1.413f ? pow( abs( l.b * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.b );
	return l;
}

float4 main( PS_INPUT input ) : SV_Target
{
	float3 viewRay = normalize( input.worldPosition - CameraPos.xyz );

	float scale = 0.00001f;
	float3 viewPos = CameraPos.xyz * scale;
	viewPos.y += Rg + HeightOffset;

	float r = length( viewPos );

	float mu = dot( viewPos, viewRay ) / r;
	float t = -r * mu - sqrt( r * r * ( mu * mu - 1.f ) + Rg * Rg );

	float viewSpaceDistance = ViewSpaceDistance.Sample( ViewSpaceDistanceSampler, input.uv ).x;
	bool isSceneGeometry = viewSpaceDistance > 0.f;

	if ( isSceneGeometry )
	{
		t = length( viewSpaceDistance * scale );
	}

	float3 attenuation;
	float3 inscatterColor = InscatterColor( viewPos, t, viewRay, r, mu, attenuation );
	float3 groundColor = GroundColor( viewPos, t, viewRay, r, mu, attenuation, isSceneGeometry );
	float3 sunColor = SunColor( viewPos, t, viewRay, r, mu, isSceneGeometry );

	return float4( HDR( sunColor + inscatterColor + groundColor ), 1.f );
}