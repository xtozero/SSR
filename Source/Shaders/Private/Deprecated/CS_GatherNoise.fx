RWTexture3D<float> NoiseTex1;
RWTexture3D<float> NoiseTex2;
RWTexture3D<float> NoiseTex3;
RWTexture3D<float> NoiseTex4;

RWTexture3D<float4> ResultTex;

uint NumTex;

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint3 dims;
	ResultTex.GetDimensions( dims.x, dims.y, dims.z );

	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float4 noise = 0;
		if ( NumTex > 0 )
		{
			noise.x = NoiseTex1[DTid];
		}
		
		if ( NumTex > 1 )
		{
			noise.y = NoiseTex2[DTid];
		}
		
		if ( NumTex > 2 )
		{
			noise.z = NoiseTex3[DTid];
		}

		if ( NumTex > 3 )
		{
			noise.w = NoiseTex4[DTid];
		}

		ResultTex[DTid] = noise;
	}
}