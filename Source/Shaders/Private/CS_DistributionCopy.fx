StructuredBuffer<float4> src : register( s0 );
StructuredBuffer<uint> distributer : register( s1 );
RWStructuredBuffer<float4> dest : register( u0 );

cbuffer DistributionCopyConstant : register( c0 )
{
	uint numDistribution : packoffset( c0 );
}

[numthreads(64, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	uint distributionIndex = id.x;

	if ( distributionIndex < numDistribution )
	{
		uint destIndex = distributer[distributionIndex];
		dest[destIndex] = src[distributionIndex];
	}
}