#define ATMOSPHERE_RENDERING

#include "atmosphereCommon.fxh"

static const float ISun = 100.f;
static const float HeightOffset = 0.01f;

cbuffer ATMOSPHERIC_PARAMETER : register( b4 )
{
	float4 g_cameraPos : packoffset( c0 );
	float4 g_sunDir : packoffset( c1 );
	float g_exposure : packoffset( c2 );
	float3 padding : packoffset( c2.y );
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 wolrdPos : POSITION0;
};

float3 InscatterColor( float3 x, float t, float3 viewRay, float altitude, float mu, out float3 attenuation )
{
	float3 result = 0.f;
	attenuation = 1.f;

	float d = -altitude * mu - sqrt( altitude * altitude * ( mu * mu - 1.f ) + Rt * Rt );
	if ( d > 0.f )
	{
		x += d * viewRay;
		t -= d;
		mu = ( altitude * mu + d ) / Rt;
		altitude = Rt;
	}

	static const float EPS = 0.005f;

	if ( altitude < Rg + HeightOffset + EPS )
	{
		float diff = ( Rg + HeightOffset + EPS ) - altitude;
		x -= diff * viewRay;
		t -= diff;
		mu = dot( x, viewRay ) / altitude;
		altitude = Rg + HeightOffset + EPS;
	}

	if ( altitude <= Rt )
	{
		float nu = dot( viewRay, g_sunDir.xyz );
		float muS = dot( x, g_sunDir.xyz ) / altitude;
		float4 inscatter = max( Sample4D( inscatterTex, inscatterSampler, altitude, mu, muS, nu ), 0.f );

		if ( t > 0.f )
		{
			float3 x0 = x + t * viewRay;
			float altitude0 = length( x0 );
			float mu0 = dot( x0, viewRay ) / altitude0;
			float muS0 = dot( x0, g_sunDir.xyz ) / altitude0;

			attenuation = AnalyticTransmittance( altitude, mu, t );

			if ( altitude0 > Rg + HeightOffset )
			{
				inscatter = max( inscatter - attenuation.rgbr * Sample4D( inscatterTex, inscatterSampler, altitude0, mu0, muS0, nu ), 0.f );

				float muHoriz = -sqrt( 1.f - ( Rg / altitude ) * ( Rg / altitude ) );
				if ( abs( mu - muHoriz ) < EPS )
				{
					float a = ( ( mu - muHoriz ) + EPS ) / ( 2.f * EPS );

					mu = muHoriz - EPS;
					altitude0 = sqrt( altitude * altitude + t * t + 2.f * altitude * t * mu );
					mu0 = ( altitude * mu + t ) / altitude0;

					float4 inscatter0 = Sample4D( inscatterTex, inscatterSampler, altitude, mu, muS, nu );
					float4 inscatter1 = Sample4D( inscatterTex, inscatterSampler, altitude0, mu0, muS0, nu );
					float4 inscatterA = max( inscatter0 - attenuation.rgbr * inscatter1, 0.f );

					mu = muHoriz + EPS;
					altitude0 = sqrt( altitude * altitude + t * t + 2.f * altitude * t * mu );
					mu0 = ( altitude * mu + t ) / altitude0;

					inscatter0 = Sample4D( inscatterTex, inscatterSampler, altitude, mu, muS, nu );
					inscatter1 = Sample4D( inscatterTex, inscatterSampler, altitude0, mu0, muS0, nu );
					float4 inscatterB = max( inscatter0 - attenuation.rgbr * inscatter1, 0.f );

					inscatter = lerp( inscatterA, inscatterB, a );
				}
			}
		}

		inscatter.w *= smoothstep( 0.f, 0.02f, muS );

		float phaseR = PhaseFunctionR( nu );
		float phaseM = PhaseFunctionM( nu );
		result = max( inscatter.rgb * phaseR + getMie( inscatter ) * phaseM, 0.f );
	}

	return result * ISun;
}

float3 GroundColor( float3 x, float t, float3 viewRay, float altitude, float mu, float3 attenuation )
{
	float3 result = 0.f;

	if ( t > 0.f )
	{
		float3 x0 = x + t * viewRay;
		float altitude0 = length( x0 );
		float3 n = x0 / altitude0;

		float muS = dot( n, g_sunDir.xyz );
		float3 sunLight = TransmittanceWithShadow( altitude0, muS );

		float3 groundSkyLight = Irradiance( altitude0, muS );

		float3 sceneColor = float3( 0.35f, 0.35f, 0.35f );
		float3 reflectance = sceneColor * float3( 0.2f, 0.2f, 0.2f );
		if ( altitude0 > Rg + HeightOffset ) {
			reflectance = float3( 0.4f, 0.4f, 0.4f );
		}

		float3 groundColor = reflectance * ISun / M_PI * ( max( muS, 0.f ) ) * sunLight + groundSkyLight;

		result = attenuation * groundColor;
	}

	return result;
}

float3 SunColor( float3 x, float t, float3 viewRay, float altitude, float mu )
{
	if ( t > 0.f )
	{
		return 0.f;
	}
	else
	{
		float3 transmittance = altitude <= Rt ? TransmittanceWithShadow( altitude, mu ) : 1.f;
		float isun = step( cos( M_PI / 180 ), dot( viewRay, g_sunDir.xyz ) ) * ISun;
		return transmittance * isun;
	}
}

float3 HDR( float3 l )
{
	l = l * g_exposure;
	l.r = l.r < 1.413f ? pow( abs( l.r * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.r );
	l.g = l.g < 1.413f ? pow( abs( l.g * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.g );
	l.b = l.b < 1.413f ? pow( abs( l.b * 0.38317f ), 1.f / 2.2f ) : 1.f - exp( -l.b );
	return l;
}

float4 main( PS_INPUT input ) : SV_Target
{
	float3 viewRay = normalize( input.wolrdPos - g_cameraPos.xyz );

	float scale = 0.00001f;
	float3 viewPos = g_cameraPos.xyz * scale;
	viewPos.y += Rg + HeightOffset;

	float altitude = length( viewPos );

	float mu = dot( viewPos, viewRay ) / altitude;
	float t = -altitude * mu - sqrt( altitude * altitude * ( mu * mu - 1.f ) + Rg * Rg );

	float3 attenuation;
	float3 inscatterColor = InscatterColor( viewPos, t, viewRay, altitude, mu, attenuation );
	float3 groundColor = GroundColor( viewPos, t, viewRay, altitude, mu, attenuation );
	float3 sunColor = SunColor( viewPos, t, viewRay, altitude, mu );

	return float4( HDR( sunColor + inscatterColor + groundColor ), 1.f );
}