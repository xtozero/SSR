RWTexture3D<float> NoiseTex : register( u0 );
RWTexture3D<float> ResultTex : register( u1 );

float HurstExponent;
float Amplitude;
uint Octaves;

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint3 dims;
	NoiseTex.GetDimensions( dims.x, dims.y, dims.z );

	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float gain = exp2( -HurstExponent );
		float amplitude = Amplitude;
		uint3 idx = DTid;
		for ( uint i = 0; i < Octaves; ++i )
		{
			ResultTex[DTid] += amplitude * NoiseTex[idx];
			idx *= 2.f;
			idx %= dims;
			amplitude *= gain;
		}
	}
}