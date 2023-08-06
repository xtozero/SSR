Texture2DArray<float> SrcTexture : register(t0);
RWTexture2DArray<float> ESMsTexture : register(u0);

cbuffer ESMsParameters : register( b0 )
{
	float ParameterC : packoffset( c0.x );
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	ESMsTexture[DTid] = exp( ParameterC * SrcTexture[DTid] );
}