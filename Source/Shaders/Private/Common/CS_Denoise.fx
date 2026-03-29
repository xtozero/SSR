Texture2D<float4> PrevImage;
Texture2D<float4> Image;
Texture2D<float> PrevViewSpaceDistance;
Texture2D<float> ViewSpaceDistance;
Texture2D<float2> Velocity;

SamplerState Sampler;

RWTexture2D<float4> Denoised;

int KernelRadius;
float2 ScreenSize;
float2 InvScreenSize;

float Sqr( float x )
{
    return x * x;
}

float Gaussian( float x, float sigma )
{
    return exp( -Sqr( x / sigma ) );
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float2 uv = ( DTid.xy + 0.5f ) * InvScreenSize;

    [branch]
    if ( all( uv < 1.f ) )
    {
        float totalWeight = 1.f;

        float sigma = 0.5f * float( KernelRadius );

        float3 color = Image.SampleLevel( Sampler, uv, 0 ).rgb;
        float linearDepth = ViewSpaceDistance.SampleLevel( Sampler, uv, 0 ).x;

        // Apply bilateral filter
        [loop]
        for ( int y = -KernelRadius; y <= KernelRadius; ++y )
        {
            [loop]
            for ( int x = -KernelRadius; x <= KernelRadius; ++x )
            {
                if ( ( x == 0 ) && ( y == 0 ) )
                {
                    continue;
                }

                float2 sampleUV = ( DTid.xy + float2( x, y ) + 0.5f ) * InvScreenSize;

                float sampleLinearDepth = ViewSpaceDistance.SampleLevel( Sampler, sampleUV, 0 ).x;
                float depthWeight = max( 0.f, 1.f - abs( sampleLinearDepth - linearDepth ) );

                float w = Gaussian( x, sigma ) * Gaussian( y, sigma ) * depthWeight;

                float3 sampleColor = Image.SampleLevel( Sampler, sampleUV, 0 ).rgb;
                
                color += sampleColor * w;
                totalWeight += w;
            }
        }

        color /= totalWeight;

        // Apply temporal filter
        float2 velocity = Velocity.SampleLevel( Sampler, uv, 0 );
        float2 prevUV = uv - velocity;

        if ( all( 0 <= prevUV ) && all( prevUV <= 1 ) )
        {
            float3 prevColor = PrevImage.SampleLevel( Sampler, prevUV, 0 ).rgb;

            float prevLinearDepth = PrevViewSpaceDistance.SampleLevel( Sampler, prevUV, 0 ).x;
            float w = exp( -abs( prevLinearDepth - linearDepth ) / 10 );

            color = lerp( color, prevColor, 0.9 * w );
        }

        Denoised[DTid.xy] = float4( color, 1.f );
    }
}