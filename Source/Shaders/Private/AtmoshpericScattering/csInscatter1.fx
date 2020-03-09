#include "AtmoshpericScattering/atmosphereCommon.fxh"

RWTexture3D<float4> deltaSRBuffer : register( u0 );
RWTexture3D<float4> deltaSMBuffer : register( u1 );

void Integrand( float r, float mu, float muS, float nu, float t, out float3 ray, out float3 mie )
{
	ray = 0.f;
	mie = 0.f;
	float ri = sqrt( r * r + t * t + 2.f * r * mu * t );
	float muSi = ( nu * t + muS * r ) / ri;
	ri = max( Rg, ri );

	// if angle between zenith and sun smaller than angle to horizon return ray and mie value
	if ( muSi >= -sqrt( 1.f - Rg * Rg / ( ri * ri ) ) )
	{
		float3 ti = Transmittance( r, mu, t ) * Transmittance( ri, muSi );
		ray = exp( -( ri - Rg ) / HR ) * ti;
		mie = exp( -( ri - Rg ) / HM ) * ti;
	}
}

void Inscatter( float r, float mu, float muS, float nu, out float3 ray, out float3 mie )
{
	ray = 0.f;
	mie = 0.f;
	float dx = limit( r, mu ) / float( INSCATTER_INTEGRAL_SAMPLES );
	float3 rayi;
	float3 miei;
	Integrand( r, mu, muS, nu, 0.f, rayi, miei );
	for ( int i = 1; i <= INSCATTER_INTEGRAL_SAMPLES; ++i )
	{
		float xj = float( i ) * dx;
		float3 rayj;
		float3 miej;
		Integrand( r, mu, muS, nu, xj, rayj, miej );
		ray += ( rayi + rayj ) * 0.5f * dx;
		mie += ( miei + miej ) * 0.5f * dx;
		rayi = rayj;
		miei = miej;
	}

	ray *= betaR;
	mie *= betaMSca;
}

[numthreads( RES_MU_S, RES_MU / INSCATTER1_GROUP_Y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float r;
	float4 dhdH;
	GetRdhdH( DTid.z, r, dhdH );

	float mu, muS, nu;
	GetMuMuSNu( DTid, r, dhdH, mu, muS, nu );

	float3 ray, mie;
	Inscatter( r, mu, muS, nu, ray, mie );

	deltaSRBuffer[DTid] = float4( ray, 0.f );
	deltaSMBuffer[DTid] = float4( mie, 0.f );
}
