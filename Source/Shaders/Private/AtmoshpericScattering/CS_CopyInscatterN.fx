#include "AtmoshpericScattering/AtmosphereCommon.fxh"

RWStructuredBuffer<float4> Inscatter : register( u0 );

[numthreads( RES_MU_S, RES_MU / INSCATTERN_GROUP_Y, 1 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float r;
	float4 dhdH;
	GetRdhdH( DTid.z, r, dhdH );

	float mu, muS, nu;
	GetMuMuSNu( DTid, r, dhdH, mu, muS, nu );

	float3 uvw = ( DTid.xyz + 0.5f ) / float3( RES_MU_S * RES_NU, RES_MU, RES_R );
	float pitch = RES_MU_S * RES_NU;
	float slicePitch = pitch * RES_MU;

	Inscatter[( DTid.z * slicePitch ) + ( DTid.y * pitch ) + DTid.x] += float4( DeltaSRLut.SampleLevel( DeltaSRLutSampler, uvw, 0 ).rgb / PhaseFunctionR( nu ), 0.f );
}