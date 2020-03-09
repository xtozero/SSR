#include "AtmoshpericScattering/atmosphereCommon.fxh"

static const float dphi = M_PI / float( INSCATTER_SPHERICAL_INTEGRAL_SAMPLES );
static const float dtheta = M_PI / float( INSCATTER_SPHERICAL_INTEGRAL_SAMPLES );

cbuffer SCATTERING_ORDER : register( b0 )
{
	int g_order : packoffset( c0.x );
	int g_threadGroupZ : packoffset( c0.y );
}

RWTexture3D<float4> deltaJBuffer : register( u0 );

void Inscatter( float r, float mu, float muS, float nu, out float3 raymie )
{
	r = clamp( r, Rg, Rt );
	mu = clamp( mu, -1.f, 1.f );
	muS = clamp( muS, -1.f, 1.f );
	// Using formula sum and differences of cosine
	float var = sqrt( 1.f - mu * mu ) * sqrt( 1.f - muS * muS );
	nu = clamp( nu, mu * muS - var, mu * muS + var );

	// cos( theta to the horizon )
	float cthetamin = -sqrt( 1.f - ( Rg * Rg ) / ( r * r ) );

	float3 v = float3( sqrt( 1.0 - mu * mu ), 0.f, mu );
	float sx = v.x == 0.f ? 0.f : ( nu - muS * mu ) / v.x; // https://en.wikipedia.org/wiki/Spherical_trigonometry#Derivation_of_the_cosine_rule
	float3 s = float3( sx, sqrt( max( 0.f, 1.f - sx * sx - muS * muS ) ), muS );

	raymie = 0.f;

	// Compute equation(7)
	for ( int itheta = 0; itheta < INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++itheta )
	{
		float theta = ( float( itheta ) + 0.5f ) * dtheta;
		float ctheta = cos( theta );

		float greflectance = 0.f;
		float dground = 0.f;
		float3 gtransp = 0.f;

		// if ground visible in direction w compute transparency gtransp between x and ground
		if ( ctheta < cthetamin )
		{
			greflectance = AVERAGE_GROUND_REFLECTANCE / M_PI;
			dground = -r * ctheta - sqrt( r * r * ( ctheta * ctheta - 1.f ) + Rg * Rg );
			gtransp = Transmittance( Rg, -(r * ctheta + dground) / Rg , dground );
		}

		for ( int iphi = 0; iphi < 2 * INSCATTER_SPHERICAL_INTEGRAL_SAMPLES; ++iphi )
		{
			float phi = ( float( iphi ) + 0.5f ) * dphi;
			float dw = dtheta * dphi * sin( theta );
			float3 w = float3( cos( phi ) * sin( theta ), sin( phi ) * sin( theta ), ctheta );

			float nu1 = dot( s, w );
			float nu2 = dot( v, w );
			float pr2 = PhaseFunctionR( nu2 );
			float pm2 = PhaseFunctionM( nu2 );

			float3 gnormal = ( float3( 0.f, 0.f, r ) + dground * w ) / Rg;
			float3 girradiance = Irradiance( Rg, dot( gnormal, s ) );

			// first term = light reflected from the ground and attenuated before reaching x
			float3 raymie1 = greflectance * girradiance * gtransp;

			// second term = inscattered light = deltaS
			if ( g_order == 2 )
			{
				float pr1 = PhaseFunctionR( nu1 );
				float pm1 = PhaseFunctionM( nu1 );
				float3 ray1 = Sample4D( deltaSRTex, deltaSRSampler, r, w.z, muS, nu1 ).rgb;
				float3 mie1 = Sample4D( deltaSMTex, deltaSMSampler, r, w.z, muS, nu1 ).rgb;

				raymie1 += ray1 * pr1 + mie1 * pm1;
			}
			else
			{
				raymie1 += Sample4D( deltaSRTex, deltaSRSampler, r, w.z, muS, nu1 ).rgb;
			}

			// light coming from direction w and scattered in direction v
			// Equation 7
			raymie += raymie1 * ( betaR * exp( -( r - Rg ) / HR ) * pr2 + betaMSca * exp( -( r - Rg ) / HM ) * pm2 ) * dw;
		}
	}
}

// Because of warning X4714. reduce thread count in half
[numthreads( RES_MU_S * RES_NU / INSCATTERS_GROUP_X, RES_MU / INSCATTERS_GROUP_Y, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float r;
	float4 dhdH;
	GetRdhdH( g_threadGroupZ, r, dhdH );

	float mu, muS, nu;
	GetMuMuSNu( float3( DTid.xy, g_threadGroupZ ), r, dhdH, mu, muS, nu );

	float3 raymie;
	Inscatter( r, mu, muS, nu, raymie );

	deltaJBuffer[DTid] = float4( raymie, 0.f );
}