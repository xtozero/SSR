StructuredBuffer<float4> Src : register( t0 );
StructuredBuffer<uint> Distributer : register( t1 );
RWStructuredBuffer<float4> Dest : register( u0 );

cbuffer DistributionCopyConstant : register( b0 )
{
	uint NumDistribution : packoffset( c0 );
}

[numthreads(64, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
	uint distributionIndex = id.x;

	if ( distributionIndex < NumDistribution )
	{
		uint destIndex = Distributer[distributionIndex];
		Dest[destIndex] = Src[distributionIndex];
	}
}