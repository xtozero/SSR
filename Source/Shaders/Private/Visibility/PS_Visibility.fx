#include "Common/ViewConstant.fxh"
#include "Visibility/VisibilityCommon.fxh"

struct Input
{
	float4 position : SV_POSITION;
	uint drawcallId : DRAWCALLID;
};

struct Output
{
	uint visibility : SV_Target0;
};

Output main( Input input, uint triangleIndex : SV_PrimitiveID )
{
	Output output = (Output)0;
	output.visibility = EncodeVisibility( triangleIndex, input.drawcallId );

	return output;
}