#include "atmosphereCommon.fxh"

RWStructuredBuffer<float4> inscatterBuffer : register( u0 );

[numthreads( RES_MU_S, RES_MU / INSCATTER1_GROUP_Y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float3 uvw = ( DTid.xyz + 0.5f ) / float3( RES_MU_S * RES_NU, RES_MU, RES_R );
	float4 ray = deltaSRTex.SampleLevel( deltaSRSampler, uvw, 0 );
	float4 mie = deltaSMTex.SampleLevel( deltaSMSampler, uvw, 0 );

	float pitch = RES_MU_S * RES_NU;
	float slicePitch = pitch * RES_MU;

	inscatterBuffer[( DTid.z * slicePitch ) + ( DTid.y * pitch ) + DTid.x] = float4( ray.rgb, mie.r );
}