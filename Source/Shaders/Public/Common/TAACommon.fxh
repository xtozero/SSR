#include "Constants.fxh"
#include "ViewConstant.fxh"

float4 ApplyTAAJittering( float4 clipSpace )
{
#if TAA == 1
	int idx = FrameCount % MAX_HALTON_SEQUENCE;

	float2 jitter = HALTON_SEQUENCE[idx].xy;
	jitter.x = ( jitter.x - 0.5f ) / ViewportDimensions.x * 2.f;
	jitter.y = ( jitter.y - 0.5f ) / ViewportDimensions.y * 2.f;

	clipSpace.xy += jitter * clipSpace.w;
#endif

	return clipSpace;
}