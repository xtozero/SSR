#define KernelSize 7
#define KernelRadius ( KernelSize / 2 )

cbuffer GaussianBlurParameter : register( b0 )
{
	float4 KernelBuffer[32] : packoffset( c0 );
}

static float Kernel[128] = (float[128])KernelBuffer;

Texture2DArray<float> SrcTexture : register( t0 );
RWTexture2DArray<float> DestTexture : register( u0 );

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float result = 0.f;
    int3 center = DTid;

    uint3 dims = (uint3)0;
    SrcTexture.GetDimensions( dims.x, dims.y, dims.z );

    int kernelIdx = 0;

    [unroll]
    for ( int step = -KernelRadius; step <= KernelRadius; ++step )
    {
#if Vertical == 1
        int3 sampleIdx = center + int3(0, step, 0);
#else
        int3 sampleIdx = center + int3(step, 0, 0);
#endif
        [branch]
        if ( all( 0 <= sampleIdx ) && all( sampleIdx < dims ) )
        {
            result += Kernel[kernelIdx] * SrcTexture[sampleIdx];
        }

        ++kernelIdx;
    }

    DestTexture[DTid] = result;
}