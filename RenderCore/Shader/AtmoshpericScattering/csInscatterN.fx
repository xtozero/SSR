#include "atmosphereCommon.fxh"

RWTexture3D<float4> deltaSRBuffer : register( u0 );

float3 Integrand( float r, float mu, float muS, float nu, float t )
{
	float ri = sqrt( r * r + t * t + 2.f * r * t * mu );
	float mui = ( r * mu + t ) / ri;
	float muSi = ( nu * t + muS * r ) / ri;
	return Sample4D( deltaJTex, deltaJSampler, ri, mui, muSi, nu ).rgb * Transmittance( r, mu, t );
}

float3 Inscatter( float r, float mu, float muS, float nu )
{
	float3 raymie = 0.f;
	float dx = limit( r, mu ) / float( INSCATTER_INTEGRAL_SAMPLES );
	float xi = 0.f;
	float3 raymiei = Integrand( r, mu, muS, nu, 0.f );
	for ( int i = 0; i < INSCATTER_INTEGRAL_SAMPLES; ++i )
	{
		float xj = float( i ) * dx;
		float3 raymiej = Integrand( r, mu, muS, nu, xj );
		raymie += ( raymiei + raymiej ) * 0.5f * dx;
		xi = xj;
		raymiei = raymiej;
	}

	return raymie;
}

[numthreads( RES_MU_S, RES_MU / INSCATTERN_GROUP_Y, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float r;
	float4 dhdH;
	GetRdhdH( DTid.z, r, dhdH );

	float mu, muS, nu;
	GetMuMuSNu( DTid, r, dhdH, mu, muS, nu );

	deltaSRBuffer[DTid] = float4( Inscatter( r, mu, muS, nu ), 0.f );
}