#include "AtmoshpericScattering/atmosphereCommon.fxh"

static const float dphi = M_PI / float( IRRADIANCE_INTEGRAL_SAMPLES );
static const float dtheta = M_PI / float( IRRADIANCE_INTEGRAL_SAMPLES );

cbuffer SCATTERING_ORDER : register( b0 )
{
	int g_order : packoffset( c0 );
}

RWTexture2D<float4> irradianceBuffer : register( u0 );

[numthreads( IRRADIANCE_W / IRRADIANCE_GROUP_X, IRRADIANCE_H / IRRADIANCE_GROUP_Y, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float r, muS;
	GetIrradianceRMuS( DTid, r, muS );

	float3 s = float3( max( sqrt( 1.0 - muS * muS ), 0.f ), 0.f, muS );

	float3 result = 0.f;

	for ( int iphi = 0; iphi < 2 * IRRADIANCE_INTEGRAL_SAMPLES; ++iphi )
	{
		float phi = ( float( iphi ) + 0.5f ) * dphi;
		for ( int itheta = 0; itheta < IRRADIANCE_INTEGRAL_SAMPLES / 2; ++itheta )
		{
			float theta = ( float( itheta ) + 0.5f ) * dtheta;
			float dw = sin( theta ) * dtheta * dphi;
			float3 w = float3( cos( phi ) * sin( theta ), sin( phi ) * sin( theta ), cos( theta ) );
			float nu = dot( s, w );
			if ( g_order == 2 )
			{
				float pr = PhaseFunctionR( nu );
				float pm = PhaseFunctionM( nu );
				float3 ray = Sample4D( deltaSRTex, deltaSRSampler, r, w.z, muS, nu ).rgb;
				float3 mie = Sample4D( deltaSMTex, deltaSMSampler, r, w.z, muS, nu ).rgb;
				result += ( ray * pr + mie * pm ) * w.z * dw;
			}
			else
			{
				result += Sample4D( deltaSRTex, deltaSRSampler, r, w.z, muS, nu ).rgb * w.z * dw;
			}
		}
	}

	irradianceBuffer[DTid.xy] = float4( result, 0.f );
}