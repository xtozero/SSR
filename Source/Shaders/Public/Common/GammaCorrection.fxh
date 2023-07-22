float4 MoveLinearSpace( float4 color )
{
	return float4( pow( saturate( color.xyz ), 2.2 ), color.a );
}

float4 MoveGammaSapce( float4 color )
{
	return float4( pow( saturate( color.xyz ), 0.45 ), color.a );
}