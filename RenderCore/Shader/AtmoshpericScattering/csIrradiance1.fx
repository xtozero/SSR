#include "atmosphereCommon.fxh"

RWTexture2D<float4> irradiance1Buffer : register(u0);

[numthreads( IRRADIANCE_W / IRRADIANCE_GROUP_X, IRRADIANCE_H / IRRADIANCE_GROUP_Y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float r, muS;
	GetIrradianceRMuS( DTid, r, muS );
	irradiance1Buffer[DTid.xy] = float4( Transmittance( r, muS ) * max( muS, 0.f ), 0.f );
}