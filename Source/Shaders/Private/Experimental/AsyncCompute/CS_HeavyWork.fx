RWTexture2D<float4> Output : register( u0 );

float Random2D( float2 v )
{
	return frac( sin( dot( v.xy, float2( 12.9898f, 78.233f ) ) ) * 43758.5453123f );
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    [loop]
    for ( int i = 0; i < 64; ++i )
    {
        float value = Random2D( DTid.xy + float2( i, i ) );
        Output[DTid.xy] += float4( value, value, value, 1 );
    }
}