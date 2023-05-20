#include "Atmosphere/AtmosphereCommon.fxh"
#include "Common/ViewConstant.fxh"

static const float4 StratusGradient = float4( 0.f, 0.1f, 0.2f, 0.3f );
static const float4 StratocumulusGradient = float4( 0.02f, 0.2f, 0.48f, 0.625f );
static const float4 CumulusGradient = float4( 0.f, 0.1625f, 0.88f, 0.98f );
static const float CloudTopOffset = 750.f;

static const float3 NoiseKernel[6] = {
	float3( 0.38051305f,  0.92453449f, -0.02111345f ),
	float3( -0.50625799f, -0.03590792f, -0.86163418f ),
	float3( -0.32509218f, -0.94557439f,  0.01428793f ),
	float3( 0.09026238f, -0.27376545f,  0.95755165f ),
	float3( 0.28128598f,  0.42443639f, -0.86065785f ),
	float3( -0.16852403f,  0.14748697f,  0.97460106f )
};

static const float BayerFactor = 1.f / 16.f;

static const float BayerFilter[16] = {
	0.f * BayerFactor, 8.f * BayerFactor, 2.f * BayerFactor, 10.f * BayerFactor,
	12.f * BayerFactor, 4.f * BayerFactor, 14.f * BayerFactor, 6.f * BayerFactor,
	3.f * BayerFactor, 11.f * BayerFactor, 1.f * BayerFactor, 9.f * BayerFactor,
	15.f * BayerFactor, 7.f * BayerFactor, 13.f * BayerFactor, 5.f * BayerFactor
};

Texture2D ViewSpaceDistance : register( t3 );
SamplerState ViewSpaceDistanceSampler : register( s3 );

Texture3D BaseCloudShape : register( t4 );
Texture3D DetailCloudShape : register( t5 );
Texture2D WeatherMap : register( t6 );

SamplerState BaseCloudSampler : register( s4 );
SamplerState WeatherSampler : register( s5 );

cbuffer VolumetricCloudRenderParameter : register( b2 )
{
	float3 SphereRadius : packoffset( c0 ); // x : earth, y : inner, z : outer
	float LightAbsorption : packoffset( c0.w );
	float3 CameraPos : packoffset( c1 );
	float DensityScale : packoffset( c1.w );
	float4 LightPosOrDir : packoffset( c2 );
	float4 CloudColor : packoffset( c3 );
	float3 WindDirection : packoffset( c4 );
	float WindSpeed : packoffset( c4.w );
	float Crispiness : packoffset( c5 );
	float Curliness : packoffset( c5.y );
	float DensityFactor : packoffset( c5.z );
}

#define SphereCenter float3( 0.f, -SphereRadius.x, 0.f )
#define EarthRadius SphereRadius.x
#define SphereInnerRadius (EarthRadius + SphereRadius.y)
#define SphereOuterRadius (EarthRadius + SphereRadius.z)

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION0;
	float3 viewRay : POSITION1;
	float3 worldRay : POSITION2;
	float2 uv : TEXCOORD0;
};

float2 RayAndBox( float3 boundMin, float3 boundMax, float3 rayOrigin, float3 invRayDir )
{
	float3 t0 = ( boundMin - rayOrigin ) * invRayDir;
	float3 t1 = ( boundMax - rayOrigin ) * invRayDir;
	float3 tMin = min( t0, t1 );
	float3 tMax = max( t0, t1 );

	float minComponent = max( tMin.x, max( tMin.y, tMin.z ) );
	float maxComponent = min( tMax.x, min( tMax.y, tMax.z ) );

	return float2( minComponent, maxComponent );
}

bool RayAndSphere( float3 sphereOrigin, float radius, float3 rayOrigin, float3 rayDir, out float t )
{
	float3 toOrigin = rayOrigin - sphereOrigin;
	float b = dot( rayDir, toOrigin );
	float c2 = dot( toOrigin, toOrigin );

	float deter = b * b - ( c2 - radius * radius );
	if ( deter < 0.f )
	{
		return false;
	}

	float sqrtDeter = sqrt( deter );
	float t1 = -b - sqrtDeter;
	float t2 = -b + sqrtDeter;

	t = max( t1, t2 );
	if ( t < 0.f )
	{
		return false;
	}

	return true;
}

bool RayAndGround( PS_INPUT input )
{
	float3 viewRay = normalize( input.worldPosition - CameraPos );

	float scale = 0.00001f;
	float3 viewPos = CameraPos * scale;
	viewPos.y += Rg + HeightOffset;

	float r = length( viewPos );

	float mu = dot( viewPos, viewRay ) / r;
	float t = -r * mu - sqrt( r * r * ( mu * mu - 1.f ) + Rg * Rg );

	return t > 0.f;
}

bool RayAndTroposphere( float3 rayOrigin, float3 rayDir, out float2 tMinMax )
{
	float tInner = 0.f;
	if ( RayAndSphere( SphereCenter, SphereInnerRadius, rayOrigin, rayDir, tInner ) == false )
	{
		return false;
	}

	float tOuter = 0.f;
	if ( RayAndSphere( SphereCenter, SphereOuterRadius, rayOrigin, rayDir, tOuter ) == false )
	{
		return false;
	}

	tMinMax.x = min( tInner, tOuter );
	tMinMax.y = max( tInner, tOuter );

	return true;
}

float GetHeightFractionForPoint( float3 inPosition )
{
	float heightFraction = ( length( inPosition - SphereCenter ) - SphereInnerRadius ) / ( SphereOuterRadius - SphereInnerRadius );
	return heightFraction;
}

float Remap( float originalValue, float originalMin, float originalMax, float newMin, float newMax )
{
	return newMin + ( ( ( originalValue - originalMin ) / ( originalMax - originalMin ) ) * ( newMax - newMin ) );
}

float2 GetSphericalUV( float3 worldPos )
{
	return worldPos.xz / SphereInnerRadius + 0.5f;
}

float2 GetWeatherData( float2 uv )
{
	return WeatherMap.Sample( WeatherSampler, uv ).rg;
}

float GetDensityHeightGradientForPoint( float3 position, float cloudType )
{
	float heightFraction = GetHeightFractionForPoint( position );

	float stratusFactor = 1.f - clamp( cloudType * 2.f, 0.f, 1.f );
	float stratoCumulusFactor = 1.0 - abs( cloudType - 0.5f ) * 2.f;
	float cumulusFactor = clamp( cloudType - 0.5f, 0.f, 1.f ) * 2.f;

	float4 baseGradient = stratusFactor * StratusGradient + stratoCumulusFactor * StratocumulusGradient + cumulusFactor * CumulusGradient;
	return smoothstep( baseGradient.x, baseGradient.y, heightFraction ) - smoothstep( baseGradient.z, baseGradient.w, heightFraction );
}

float SampleCloudDensity( float3 position, bool expensive )
{
	float heightFraction = GetHeightFractionForPoint( position );
	float3 movingPosition = position + ( heightFraction * WindDirection * CloudTopOffset + WindDirection * TotalTime * WindSpeed );
	float2 uv = GetSphericalUV( position );
	float2 movingUV = GetSphericalUV( movingPosition );

	if ( heightFraction < 0.f || heightFraction > 1.f )
	{
		return 0.f;
	}

	float4 lowFreq = BaseCloudShape.SampleLevel( BaseCloudSampler, float3( uv * Crispiness, heightFraction ), 0 );
	float fbm = ( lowFreq.g * 0.625f ) +
				( lowFreq.b * 0.25f ) +
				( lowFreq.a * 0.125f );

	float baseCloud = Remap( lowFreq.r, -( 1.f - fbm ), 1.f, 0.f, 1.f );

	float2 weatherData = GetWeatherData( movingUV );

	float gradient = GetDensityHeightGradientForPoint( position, 1.f );
	baseCloud *= ( heightFraction == 0.f ) ? 0.f : ( gradient / heightFraction );

	float cloudCoverage = weatherData.r * DensityScale;
	baseCloud = Remap( baseCloud, cloudCoverage, 1.f, 0.f, 1.f );
	baseCloud *= cloudCoverage;

	float finalCloud = baseCloud;
	if ( expensive )
	{
		float3 highFreq = DetailCloudShape.SampleLevel( BaseCloudSampler, float3( movingUV * Crispiness, heightFraction ) * Curliness, 0 ).rgb;
		fbm = ( highFreq.r * 0.625f ) +
			  ( highFreq.g * 0.25f ) +
			  ( highFreq.b * 0.125f );

		float highFreqNoiseModifier = lerp( fbm, 1.f - fbm, saturate( heightFraction * 10.f ) );

		finalCloud = finalCloud - highFreqNoiseModifier * ( 1.0 - finalCloud );
		finalCloud = Remap( finalCloud * 2.0, highFreqNoiseModifier * 0.2f, 1.f, 0.f, 1.f );
	}

	return saturate( finalCloud );
}

float HenyeyGreensteinPhase( float cosAngle, float g )
{
	float g2 = g * g;
	return ( ( 1.f - g2 ) / pow( abs( 1.f + g2 - 2.f * g * cosAngle ), 3.f / 2.f ) ) / ( 4.f * 3.1415f );
}

float Beer( float density )
{
	return exp( -density );
}

float Powder( float density )
{
	return 1.f - exp( -density * 2.f );
}

float LightEnergy( float cosAngle, float density )
{
	return 2.f * Beer( density ) * Powder( density ) * HenyeyGreensteinPhase( cosAngle, 0.2f );
}

float RaymarchToLight( float3 pos, float3 dir, float3 lightDir, float stepSize )
{
	float3 startPos = pos;
	float ds = stepSize * 0.6f;
	float3 rayStep = lightDir * ds;
	float coneRadius = 1.f;
	float density = 0.f;
	float coneDensity = 0.f;
	float invDepth = 1.f / ds;
	float sigma_ds = -ds * LightAbsorption;

	float T = 1.f;

	for ( int i = 0; i < 6; ++i )
	{
		float3 posInCone = startPos + coneRadius * NoiseKernel[i] * float( i );

		float heightFraction = GetHeightFractionForPoint( posInCone );
		if ( heightFraction >= 0.f )
		{
			float cloudDensity = SampleCloudDensity( posInCone, density > 0.3f );
			if ( cloudDensity > 0.f )
			{
				float Ti = exp( cloudDensity * sigma_ds );
				T *= Ti;
				density += cloudDensity;
			}
		}

		startPos += rayStep;
		coneRadius += 1.f / 6.f;
	}

	return T;
}

float Raymarch( PS_INPUT input, float3 startPos, float3 endPos, out float3 color )
{
	float3 path = endPos - startPos;

	int sampleCount = 64;

	float stepSize = length( path ) / sampleCount;
	float3 pathDir = normalize( path );
	float3 stepVector = pathDir * stepSize;

	float3 pos = startPos;

	uint a = uint( input.uv.x * 1024 ) % 4;
	uint b = uint( input.uv.y * 768 ) % 4;
	pos += stepVector * BayerFilter[a * 4 + b];

	float3 result = 0.f;

	float3 lightColor = float3( 1.f, 0.99995f, 0.90193f );

	float3 lightDir = normalize( LightPosOrDir.xyz - ( pos * LightPosOrDir.w ) );

	float ldotv = dot( lightDir, pathDir );

	float T = 1.f;
	float sigma_ds = -stepSize * 0.02f;

	int i = 0;
	[loop]
	while ( i < sampleCount )
	{
		float cloudDensity = SampleCloudDensity( pos, true );
		if ( cloudDensity > 0.f )
		{
			float lightDensity = RaymarchToLight( pos, pathDir, lightDir, stepSize );
			float scattering = lerp( HenyeyGreensteinPhase( ldotv, -0.08f ), HenyeyGreensteinPhase( ldotv, 0.08f ), saturate( ldotv * 0.5 + 0.5 ) );
			scattering = max( scattering, 1.f );
			float powderTerm = Powder( cloudDensity );
			powderTerm = 1.f;

			float3 S = 0.6f * lerp( lerp( CloudColor.rgb * 1.8f, float3( 0.f, 0.f, 0.f ), 0.2f ), scattering * lightColor, powderTerm * lightDensity ) * cloudDensity;
			float dTrans = exp( cloudDensity * sigma_ds );
			float3 Sint = ( S - S * dTrans ) * ( 1.f / cloudDensity );

			result += T * Sint;
			T *= dTrans;
		}

		if ( T <= 1e-1 )
		{
			break;
		}

		pos += stepVector;
		++i;
	}

	color = result.rgb;
	return 1.f - T;
}

float4 main( PS_INPUT input ) : SV_Target0
{
	if ( RayAndGround( input ) )
	{
		return float4( 0.f, 0.f, 0.f, 0.f );
	}

	float viewSpaceDistance = ViewSpaceDistance.Sample( ViewSpaceDistanceSampler, input.uv ).x;
	float t = viewSpaceDistance;

	float alpha = 1.f;
	float3 rayDir = normalize( input.worldRay );
	float2 tMinMax = 0.f;
	bool intersect = RayAndTroposphere( CameraPos, rayDir, tMinMax );

	if ( ( ( viewSpaceDistance != 0.f ) && ( tMinMax.x > t ) ) || ( intersect == false ) )
	{
		return float4( 0.f, 0.f, 0.f, 0.f );
	}
	
	float3 cloudColor = 0.f;
	float3 startPos = CameraPos + rayDir * tMinMax.x;
	float3 endPos = CameraPos + rayDir * tMinMax.y;
	float density = Raymarch( input, startPos, endPos, cloudColor );

	cloudColor = cloudColor * 1.8f - 0.1f;
	return float4( cloudColor, density );
};