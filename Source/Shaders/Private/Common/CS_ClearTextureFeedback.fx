int NumElement;
RWStructuredBuffer<int> TextureFeedback;

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if ( DTid.x < NumElement )
	{
		TextureFeedback[DTid.x] = 1;
	}
}