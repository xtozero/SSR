float4 MoveLinearSpace( float4 color )
{
	return float4( pow( color.rgb, 2.2 ), color.a );
}

float4 MoveGammaSpace( float4 color )
{
	return float4( pow( color.rgb, 1 / 2.2 ), color.a );
}