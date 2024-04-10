#include "Common/Constants.fxh"

static const float3 F0 = (float3)0.04f;

float3 FresnelSchlick( float cosTheta, float3 f0 )
{
	return f0 + ( 1.f - f0 ) * pow( saturate( 1.f - cosTheta ), 5.f );
}

float3 FresnelSchlick( float cosTheta, float3 f0, float roughness )
{
	return f0 + ( max( (float3)( 1.f - roughness ), f0 ) - f0 ) * pow( saturate( 1.f - cosTheta ), 5.f );
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

float VanDerCorput( uint bits )
{
	bits = ( bits << 16 ) | ( bits >> 16 );
    bits = ( ( bits & 0x55555555 ) << 1 ) | ( ( bits & 0xAAAAAAAA ) >> 1 );
    bits = ( ( bits & 0x33333333 ) << 2 ) | ( ( bits & 0xCCCCCCCC ) >> 2 );
    bits = ( ( bits & 0x0F0F0F0F ) << 4 ) | ( ( bits & 0xF0F0F0F0 ) >> 4 );
    bits = ( ( bits & 0x00FF00FF ) << 8 ) | ( ( bits & 0xFF00FF00 ) >> 8 );
	return float( bits ) * 2.3283064365386963e-10; // 0x100000000
}

float2 Hammersley( uint i, uint n )
{
	return float2( float( i ) / float( n ), VanDerCorput( i ) );
}

float3 ImportanceSampleGGX( float2 xi, float3 normal, float roughness )
{
	float a = roughness * roughness;

	float phi = 2.f * PI * xi.x;
	float cosTheta = sqrt( ( 1.f - xi.y ) / ( 1.f + ( a * a - 1.f ) * xi.y ) );
	float sinTheta = sqrt( 1.f - cosTheta * cosTheta );

	float3 h = { cos( phi ) * sinTheta, sin( phi ) * sinTheta, cosTheta };

	float3 up = ( abs( normal.y ) < 0.999f ) ? float3( 0.f, 1.f, 0.f ) : float3( 0.f, 0.f, 1.f );
    float3 right = normalize( cross( up, normal ) );
    up = normalize( cross( normal, right ) );
    
    float3x3 toWorld = float3x3( right, up, normal );
	
	return normalize( mul( h, toWorld ) );
}

float3 PrefilterSpecular( float3 normal, float roughness, TextureCube envMap, SamplerState envMapSampler )
{
	float3 viewDirection = normal;

	const uint SampleCount = 1024;
	float totalWeight = 0.f;
	float3 prefilteredColor = (float3)0.f;

	for ( uint i = 0; i < SampleCount; ++i )
	{
		float2 xi = Hammersley( i, SampleCount );
		float3 halfWay = ImportanceSampleGGX( xi, normal, roughness );
		float3 toLight = normalize( 2.f * dot( viewDirection, halfWay ) * halfWay - viewDirection );

		float ndotl = saturate( dot( normal, toLight ) );
		if (ndotl > 0.f)
		{
			prefilteredColor += envMap.SampleLevel( envMapSampler, toLight, 0 ).rgb * ndotl;
			totalWeight += ndotl;
		}
	}

	return prefilteredColor / totalWeight;
}

float GeometrySchlickGGXForIBL( float ndotv, float roughness )
{
	float a = roughness;
	float k = ( a * a ) / 2.f;

	float num = ndotv;
	float denom = ndotv * ( 1.f - k ) + k;

	return num / denom;
}

float GemoetrySmithForIBL( float ndotl, float ndotv, float roughness )
{
	float ggx1 = GeometrySchlickGGXForIBL( ndotl, roughness );
	float ggx2 = GeometrySchlickGGXForIBL( ndotv, roughness );

	return ggx1 * ggx2;
}

float2 IntegrateBRDF( float ndotv, float roughness )
{
	float3 viewDirection = { sqrt( 1.f - ndotv * ndotv ), 0.f, ndotv };

	float a = 0.f;
	float b = 0.f;

	float3 normal = { 0.f, 0.f, 1.f };

	const uint SampleCount = 1024;
	for ( uint i = 0; i < SampleCount; ++i )
	{
		float2 xi = Hammersley( i, SampleCount );
		float3 halfWay = ImportanceSampleGGX( xi, normal, roughness );
		float3 toLight = normalize( 2.f * dot( viewDirection, halfWay ) * halfWay - viewDirection );

		float ndotl = saturate( toLight.z );
		float ndoth = saturate( halfWay.z );
		float vdoth = saturate( dot( viewDirection, halfWay ) );

		if ( ndotl > 0.f )
		{
			float g = GemoetrySmithForIBL( ndotl, ndotv, roughness );
			float gVis = ( g * vdoth ) / ( ndoth * ndotv );
			float fc = pow( 1.f - vdoth, 5.f );

			a += ( 1.f - fc ) * gVis;
			b += fc * gVis;
		}
	}

	a /= (float)SampleCount;
	b /= (float)SampleCount;

	return float2( a, b );
}