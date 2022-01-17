#include "AtmosphereConstant.h"

static const float Rg = 6360.f;	// Altitude ground( km ) 
static const float Rt = 6420.f;	// Altitude atmosphere( km )
static const float RL = 6421.f;	// Altitude limit( km )

static const float AVERAGE_GROUND_REFLECTANCE = 0.1;

// Rayleigh
static const float HR = 8.f; // HeightScale Rayleigh
static const float3 betaR = float3( 5.8e-3f, 1.35e-2f, 3.31e-2f ); // REK 04, Table 3

// Mie
static const float HM = 1.2f; // HeightScale Mie
static const float3 betaMSca = 4e-3f;
static const float3 betaMEx = betaMSca / 0.9f; // Figure 6 BetaMieScattering / BetaMieExtinction = 0.9
static const float mieG = 0.8f;

static const int TRANSMITTANCE_INTEGRAL_SAMPLES = 500;
static const int INSCATTER_INTEGRAL_SAMPLES = 50;
static const int IRRADIANCE_INTEGRAL_SAMPLES = 32;
static const int INSCATTER_SPHERICAL_INTEGRAL_SAMPLES = 16;

static const float M_PI = 3.141592657;

#ifdef ATMOSPHERE_RENDERING
Texture2D TransmittanceLut : register( t0 );
Texture2D IrradianceLut : register( t1 );
Texture3D InscatterLut : register( t2 );

SamplerState TransmittanceLutSampler : register( s0 );
SamplerState IrradianceLutSampler : register( s1 );
SamplerState InscatterLutSampler : register( s2 );
#else
Texture2D TransmittanceLut : register( t0 );
Texture2D DeltaELut : register( t1 );
Texture3D DeltaSRLut : register( t2 );
Texture3D DeltaSMLut : register( t3 );
Texture3D DeltaJLut : register( t4 );

SamplerState TransmittanceLutSampler : register( s0 );
SamplerState DeltaELutSampler : register( s1 );
SamplerState DeltaSRLutSampler : register( s2 );
SamplerState DeltaSMLutSampler : register( s3 );
SamplerState DeltaJLutSampler : register( s4 );
#endif

#define TRANSMITTANCE_NON_LINEAR
#define INSCATTER_NON_LINEAR

// r : altitude
// mu : cos( view ray zenith angle at view ray origin )
float limit( float r, float mu )
{
	float dout = -r * mu + sqrt( r * r * ( mu * mu - 1.f ) + RL * RL );
	float delta2 = r * r * ( mu * mu - 1.f ) + Rg * Rg;
	if ( delta2 >= 0.f )
	{
		float din = -r * mu - sqrt( delta2 );
		if ( din > 0.f )
		{
			dout = min( dout, din );
		}
	}
	return dout;
}

#ifndef NO_TRANSMITTANCE_FUNCTION
float2 GetTransmittanceUV( float r, float mu )
{
	float uR, uMu;
#ifdef TRANSMITTANCE_NON_LINEAR
	uR = sqrt( ( r - Rg ) / ( Rt - Rg ) );
	uMu = atan( ( mu + 0.15f ) / ( 1.f + 0.15f ) * tan( 1.5f ) ) / 1.5f;
#else
	uR = ( r - Rg ) / ( Rt - Rg );
	uMu = ( mu + 0.15f ) / ( 1.f + 0.15f );
#endif
	return float2( uMu, uR );
}

float3 Transmittance( float r, float mu )
{
	float2 uv = GetTransmittanceUV(r, mu);
	return TransmittanceLut.SampleLevel( TransmittanceLutSampler, uv, 0 ).rgb;
}

float3 TransmittanceWithShadow( float r, float mu )
{
	return mu < -sqrt( 1.0 - ( Rg / r ) * ( Rg / r ) ) ? 0.f : Transmittance( r, mu );
}

// transmittance of atmosphere between x and x0
// d : distance between x and x0
float3 Transmittance( float r, float mu, float d )
{
	float3 result;
	float r1 = sqrt( r * r + d * d + 2.f * r * mu * d );
	float mu1 = ( r * mu + d ) / r1;
	if ( mu > 0.f )
	{
		result = min( Transmittance( r, mu ) / Transmittance( r1, mu1 ), 1.f );
	}
	else
	{
		result = min( Transmittance( r1, -mu1 ) / Transmittance( r, -mu ), 1.f );
	}
	return result;
}
#endif

void GetRdhdH( int layer, out float r, out float4 dhdH )
{
	// The Square of the distance to top atmosphere boundary for a horizontal ray at ground level
	float h2 = Rt * Rt - Rg * Rg;

	r = float( layer ) / float( RES_R - 1.f );
	r = r * r;
	r = sqrt( Rg * Rg + r * h2 ) + ( layer == 0 ? 0.01f : ( layer == RES_R - 1  ? -0.001f : 0.f ) );

	// The Square of the distance to the horizon
	float rho = sqrt( r * r - Rg * Rg );

	dhdH.x = Rt - r;
	dhdH.y = rho + sqrt( h2 );
	dhdH.z = r - Rg;
	dhdH.w = rho;
}

// muS : cos( sun zenith angle )
float2 GetIrradianceUV( float r, float muS )
{
	float uR = ( r - Rg ) / ( Rt - Rg );
	float uMus = ( muS + 0.2f ) / ( 1.f + 0.2f );
	return float2( uMus, uR );
}

void GetIrradianceRMuS( uint3 DTid, out float r, out float muS )
{
	r = Rg + DTid.y / ( float( IRRADIANCE_H ) - 1.f ) * ( Rt - Rg );
	muS = -0.2f + DTid.x / ( float( IRRADIANCE_W ) - 1.f ) * ( 1.f + 0.2f );
}

// nu : cos( angle between sun and view ray )
float4 Sample4D( Texture3D tex, SamplerState texSampler, float r, float mu, float muS, float nu )
{
	float H = sqrt( Rt * Rt - Rg * Rg );
	float rho = sqrt( r * r - Rg * Rg );

	float uR = 0.5f / float(RES_R) + rho / H * (1.f - 1.f / float( RES_R ));

#ifdef INSCATTER_NON_LINEAR
	float rmu = r * mu;
	float delta = rmu * rmu - r * r + Rg * Rg;
	float4 cst = rmu < 0.f && delta > 0.f ? float4( 1.f, 0.f, 0.f, 0.5 - 0.5 / float( RES_MU ) ) : float4( -1.f, H * H, H, 0.5 + 0.5 / float( RES_MU ) );
	float uMu = cst.w + ( rmu * cst.x + sqrt( delta + cst.y ) ) / ( rho + cst.z ) * ( 0.5f - 1.f / float( RES_MU ) );
	float uMuS = 0.5f / float( RES_MU_S ) + ( atan ( max ( muS, -0.1975f ) * tan( 1.26f * 1.1f ) ) / 1.1f + ( 1.f - 0.26f ) ) * 0.5f * ( 1.f - 1.f / float( RES_MU_S ) );
#else
	float uMu = 0.5f / float( RES_MU ) + ( mu + 1.f ) / 2.f * ( 1.f - 1.f / float( RES_MU ) );
	float uMuS = 0.5f / float( RES_MU_S ) + max( muS + 0.2, 0.f ) / 1.2f * ( 1.f - 1.f / float( RES_MU_S ) );
#endif
	float lerp = ( nu + 1.f ) / 2.f * ( float( RES_NU ) - 1.f );
	float uNu = floor( lerp );
	lerp = lerp - uNu;
	return tex.SampleLevel( texSampler, float3((uNu + uMuS) / float(RES_NU), uMu, uR), 0 ) * ( 1.f - lerp ) +
			tex.SampleLevel( texSampler, float3((uNu + uMuS + 1.f) / float(RES_NU), uMu, uR), 0 ) * lerp;
}

void GetMuMuSNu( int3 DTid, float r, float4 dhdH, out float mu, out float muS, out float nu )
{
	float x = DTid.x;
	float y = DTid.y;

	muS = fmod( x, float( RES_MU_S ) ) / ( float( RES_MU_S ) - 1.f );

#ifdef INSCATTER_NON_LINEAR
	if ( y < float( RES_MU ) / 2.f )
	{
		float d = 1.f - y / ( float( RES_MU ) / 2.f - 1.f );
		d = min( max( dhdH.z, d * dhdH.w ), dhdH.w * 0.999f );
		mu = ( Rg * Rg - r * r - d * d ) / ( 2.f * r * d );
		mu = min( mu, -sqrt( 1.f - ( Rg / r ) * ( Rg / r ) ) -0.001f );
	}
	else
	{
		float d = ( y - float( RES_MU ) / 2.f ) / ( float( RES_MU ) / 2.f - 1.f );
		d = min( max( dhdH.x, d * dhdH.y ), dhdH.y * 0.999f );
		mu = ( Rt * Rt - r * r - d * d ) / ( 2.f * r * d );
	}
	muS = tan( ( 2.f * muS - 1.f + 0.26f ) * 1.1f ) / tan( 1.26f * 1.1f );
#else
	mu = -1.f + 2.f * y / ( float( RES_MU ) - 1.f );
	muS = -0.2f + muS * 1.2f;
#endif
	nu = -1.f + floor( x / float( RES_MU_S ) ) / ( float( RES_NU ) - 1.f ) * 2.f;
}

// H : height scale
float OpticalDepth( float H, float r, float mu, float d )
{
	float a = sqrt( ( 0.5 / H ) * r );
	float2 a01 = a * float2( mu, mu + d / r );
	float2 a01s = sign(a01);
	float2 a01sq = a01 * a01;
	float x = a01s.y > a01s.x ? exp( a01sq.x ) : 0.f;
	float2 y = a01s / ( 2.3193f * abs( a01 ) + sqrt( 1.52f * a01sq + 4.f ) ) * float2( 1.f, exp( -d / H * ( d / ( 2.f * r ) + mu ) ) );
	return sqrt( ( 6.2831f * H ) * r ) * exp( ( Rg - r ) / H ) * ( x + dot( y, float2( 1.f, -1.f ) ) );
}

float3 AnalyticTransmittance( float r, float mu, float d )
{
	return exp( -betaR * OpticalDepth( HR, r, mu, d ) - betaMEx * OpticalDepth( HM, r, mu, d ) );
}

#ifndef NO_IRRADIANCE_FUNCTION
#ifdef ATMOSPHERE_RENDERING
float3 Irradiance( float r, float muS )
{
	float2 uv = GetIrradianceUV( r, muS );
	return IrradianceLut.SampleLevel( IrradianceLutSampler, uv, 0 ).rgb;
}
#else
float3 Irradiance( float r, float muS )
{
	float2 uv = GetIrradianceUV( r, muS );
	return DeltaELut.SampleLevel( DeltaELutSampler, uv, 0 ).rgb;
}
#endif
#endif

float PhaseFunctionR( float mu )
{
	return ( 3.f / ( 16.f * M_PI ) ) * ( 1.f + mu * mu );
}

float PhaseFunctionM( float mu )
{
	return 3.f / ( 8.f * M_PI ) * ( 1.f - mieG * mieG ) * ( 1.f + mu * mu ) * pow( abs( 1 + ( mieG * mieG ) - 2.f * mieG * mu ), -3.f / 2.f ) / ( 2.f + mieG * mieG );
}

float3 getMie( float4 rayMie )
{
	return rayMie.rgb * rayMie.w / max( rayMie.r, 1e-4 ) * ( betaR.r / betaR );
}