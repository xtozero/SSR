#define NO_TRANSMITTANCE_FUNCTION
#include "AtmoshpericScattering/AtmosphereCommon.fxh"

// RWStructuredBuffer<float4> TransmittanceBuffer : register(u0);
RWTexture2D<float4> Transmittance : register(u0);

// H = HR ( thickness of the atmosphere for rayleigh ) or HM ( thickness of the atomosphere for mie )
// r = radius from earth center
// mu = cos( ray zenith angle at ray origin )
float OpticalDepth( float H, float r, float mu )
{
	float result = 0.f;
	float dx = limit( r, mu ) / float( TRANSMITTANCE_INTEGRAL_SAMPLES );
	float xi = 0.f;
	float yi = exp( -( r - Rg ) / H );
	for ( int i = 1; i <= TRANSMITTANCE_INTEGRAL_SAMPLES; ++i )
	{
		float xj = float( i ) * dx;
		float yj = exp( -( sqrt( r * r + xj * xj + 2.f * xj * r * mu ) - Rg ) / H ); // law of cosines
		result += ( yi + yj ) * 0.5f * dx;
		xi = xj;
		yi = yj;
	}
	
	return mu < -sqrt( 1.f - ( Rg / r ) * ( Rg / r ) ) ? 1e9f : result;
}

void GetTransmittanceRMu( uint3 DTid, out float r, out float muS )
{
	r = DTid.y / float( TRANSMITTANCE_H );
	muS = DTid.x / float( TRANSMITTANCE_W );
#ifdef TRANSMITTANCE_NON_LINEAR
	r = Rg + ( r * r ) * ( Rt - Rg );
	muS = -0.15f + tan( 1.5f * muS ) / tan( 1.5f ) * ( 1.f + 0.15f );
#else
	r = Rg + r * ( Rt - Rg );
	muS = -0.15f + muS * ( 1.f + 0.15f ); // 0 ~ ( pi/2 + epsilon )
#endif
}

[numthreads(TRANSMITTANCE_W / TRANSMITTANCE_GROUP_X, TRANSMITTANCE_H / TRANSMITTANCE_GROUP_Y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float r, muS;
	GetTransmittanceRMu( DTid, r, muS  );
	float3 depth = betaR * OpticalDepth( HR, r, muS ) + betaMEx * OpticalDepth( HM, r, muS );
	// int index = ( DTid.y * TRANSMITTANCE_W ) + DTid.x;
	// TransmittanceBuffer[index] = float4( exp( -depth ), 0.f );
	
	Transmittance[DTid.xy] = float4( exp( -depth ), 0.f );
}