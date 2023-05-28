RWTexture3D<float4> FrustumVolume : register( u0 );

float4 AccumulateScattering(float4 colorAndDensityFront, float4 colorAndDensityBack)
{
    float3 light = colorAndDensityFront.rgb + saturate(exp(-colorAndDensityFront.a)) * colorAndDensityBack.rgb;
    return float4(light.rgb, colorAndDensityFront.a + colorAndDensityBack.a);
}

void WriteOutput(uint3 uv, float4 colorAndDensity)
{
    float4 finalColor = float4(colorAndDensity.rgb, exp(-colorAndDensity.a));
    FrustumVolume[uv] = finalColor;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint3 dims;
	FrustumVolume.GetDimensions( dims.x, dims.y, dims.z );

    if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
        float4 currentSliceValue = FrustumVolume[uint3(DTid.xy, 0)];
        WriteOutput( uint3(DTid.xy, 0), currentSliceValue );

        for (uint z = 1; z < dims.z; ++z)
        {
            float4 nextSliceValue = FrustumVolume[uint3(DTid.xy, z)];
            currentSliceValue = AccumulateScattering( currentSliceValue, nextSliceValue );
            WriteOutput( uint3(DTid.xy, z), currentSliceValue );
        }
    }
}