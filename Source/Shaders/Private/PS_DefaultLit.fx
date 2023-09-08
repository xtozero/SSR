#include "Common/LightCommon.fxh"

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 viewPos : POSITION1;
	float4 projectionPos : POSITION2;
	float3 normal : NORMAL;
};

float4 main( PS_INPUT input ) : SV_Target0
{
	GeometryProperty geometry = (GeometryProperty)0;
	geometry.worldPos = input.worldPos;
	geometry.viewPos = input.viewPos;
	geometry.normal = input.normal;
    geometry.screenUV = ( input.projectionPos.xy / input.projectionPos.w ) * float2( 0.5f, -0.5f ) + 0.5f;

	return CalcLight( geometry );
}