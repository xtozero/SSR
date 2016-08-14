float2 SpheremapEncode( float3 normal )
{
	float f = sqrt( 8.f * normal.z + 8.f );
    return normal.xy / f + 0.5f;
}

float3 SpheremapDecode( float2 enc )
{
	float2 fenc = enc * 4.f - 2.f;
    float f = dot( fenc, fenc );
    float g = sqrt( 1 - f / 4.f );
    float3 n;
    n.xy = fenc * g;
    n.z = 1.f - f / 2.f;
    return n;
}

float PackedSpheremapEncode( float3 normal )
{
	float zSign = sign( normal.z );
	normal.z *= zSign;

	//0.xxxxxxxxx, 0.yyyyyyyyyy
	float2 enc = SpheremapEncode( normal );
	
	//xxxxxxx.0, yyyyyyy.0
	float2 bit7 = floor( enc * 128.f );

	//xxxxxxx.0, yyyy.yyy
	float2 split = bit7.y * 0.125f;
	split.x = floor( split.x );
	split.y -= split.x;
	
	//xxxxxxx.yyy
	float bit10 = bit7.x + split.y; 

	//0.1xxxxxxxyyy * 2^yyyy
	float mantissa = bit10.x * 0.00390625f + 0.5f; //bit10.x / 256f + 0.5f
	float exponent = split.x;
	float packed = ldexp( mantissa, exponent );

	return zSign * packed;
}

float3 PackedSpheremapDecode( float packed )
{
	float zSign = sign( packed );

	float mantissa;
	float exponent;

	mantissa = frexp( packed, exponent );
	mantissa = abs( mantissa );

	float bit10 = ( mantissa - 0.5f ) * 256.f;
	
	float2 split;
	split.x = floor( bit10 );
	split.y = bit10 - split.x;

	float2 bit7;
	bit7.x = split.x;
	bit7.y = ( exponent + split.y ) * 8.f;

	bit7 *= 0.0078125f; // bit7 /= 128,f;

	float3 decode = SpheremapDecode( bit7 );
	decode.z *= zSign;
	return decode;
}