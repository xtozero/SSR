cbuffer CB : register( b0 )
{
	unsigned int g_iCount;
}

StructuredBuffer<float> Input : register( t0 );
RWStructuredBuffer<float> Result : register( u0 );

groupshared float shared_data[128];

[numthreads(128, 1, 1)]
void main( uint3 Gid : SV_GroupID, 
		uint3 DTid : SV_DispatchThreadID, 
		uint3 GTid : SV_GroupThreadID, 
		uint GI : SV_GroupIndex )
{
	if ( DTid.x < g_iCount )
	{
		shared_data[GI] = Input[DTid.x];
	}
	else
	{
		shared_data[GI] = 0;
	}

	GroupMemoryBarrierWithGroupSync( );

	if ( GI < 64 )
	{
		shared_data[GI] += shared_data[GI + 64];
	}

	GroupMemoryBarrierWithGroupSync( );

	if ( GI < 32 )
	{
		shared_data[GI] += shared_data[GI + 32];
	}

	GroupMemoryBarrierWithGroupSync( );

	if ( GI < 16 )
	{
		shared_data[GI] += shared_data[GI + 16];
	}

	GroupMemoryBarrierWithGroupSync( );

	if ( GI < 8 )
	{
		shared_data[GI] += shared_data[GI + 8];
	}

	GroupMemoryBarrierWithGroupSync( );

	if ( GI < 4 )
	{
		shared_data[GI] += shared_data[GI + 4];
	}

	GroupMemoryBarrierWithGroupSync( );

	if ( GI < 2 )
	{
		shared_data[GI] += shared_data[GI + 2];
	}

	GroupMemoryBarrierWithGroupSync( );

	if ( GI < 1 )
	{
		shared_data[GI] += shared_data[GI + 1];
	}

	if ( GI == 0 )
	{
		Result[Gid.x] = shared_data[0];
	}
}