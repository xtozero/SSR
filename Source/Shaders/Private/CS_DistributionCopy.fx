StructuredBuffer<float4> src : register( t0 );
StructuredBuffer<uint> distributer : register( t1 );
RWStructuredBuffer<float4> dest : register( u0 );

cbuffer DistributionCopyConstant : register( b0 )
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