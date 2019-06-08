#include "atmosphereCommon.fxh"

RWStructuredBuffer<float4> irradianceBuffer : register( u0 );

[numthreads( IRRADIANCE_W / IRRADIANCE_GROUP_X, IRRADIANCE_H / IRRADIANCE_GROUP_Y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float2 uv = ( DTid.xy + float2( 0.5f, 0.5f ) ) / float2( IRRADIANCE_W, IRRADIANCE_H );
	irradianceBuffer[DTid.x + DTid.y * IRRADIANCE_W] += deltaETex.SampleLevel( deltaESampler, uv, 0 );
}