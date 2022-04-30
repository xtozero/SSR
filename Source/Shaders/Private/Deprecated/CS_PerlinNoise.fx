#include "Common/Constants.fxh"

RWTexture3D<float> NoiseTex : register( u0 );
float NoiseScale;

static const int Permutation[256] = { 151, 160, 137, 91, 90, 15, 
	131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
	190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
	102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
	5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
	223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
	129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
	251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

int Perm( int x )
{
	return Permutation[x % 256];
}

float3 Fade( float3 t )
{
	return t * t * t * ( t * ( t * 6 - 15 ) + 10 );
}

float Grad( int hash, float3 p )
{
	switch ( hash & 0xF )
	{
	case 0x0:
		return  p.x + p.y;
	case 0x1:
		return -p.x + p.y;
	case 0x2:
		return  p.x - p.y;
	case 0x3:
		return -p.x - p.y;
	case 0x4:
		return  p.x + p.z;
	case 0x5:
		return -p.x + p.z;
	case 0x6:
		return  p.x - p.z;
	case 0x7:
		return -p.x - p.z;
	case 0x8:
		return  p.y + p.z;
	case 0x9:
		return -p.y + p.z;
	case 0xA:
		return  p.y - p.z;
	case 0xB:
		return -p.y - p.z;
	case 0xC:
		return  p.y + p.x;
	case 0xD:
		return -p.y + p.z;
	case 0xE:
		return  p.y - p.x;
	case 0xF:
		return -p.y - p.z;
	}

	/*
	int h = hash & 15;
	double u = h < 8 ? p.x : p.y;
	double v;
											
	if ( h < 4 )
		v = p.y;
	else if ( h == 12 || h == 14 )
		v = p.x;
	else 
		v = p.z;

	return ( ( h & 1 ) == 0 ? u : -u ) + ( ( h & 2 ) == 0 ? v : -v );
	*/
}

int Inc( int num )
{
	return num + 1;
}

float Perlin( float3 p )
{
	int3 pi = fmod( floor( p ), 256 );
	float3 pf = p - floor( p );
	float3 uvw = Fade( pf );

	int aaa = Perm( Perm( Perm( pi.x ) + pi.y ) + pi.z );
	int aba = Perm( Perm( Perm( pi.x ) + Inc( pi.y ) ) + pi.z );
	int aab = Perm( Perm( Perm( pi.x ) + pi.y ) + Inc( pi.z ) );
	int abb = Perm( Perm( Perm( pi.x ) + Inc( pi.y ) ) + Inc( pi.z ) );
	int baa = Perm( Perm( Perm( Inc( pi.x ) ) + pi.y ) + pi.z );
	int bba = Perm( Perm( Perm( Inc( pi.x ) ) + Inc( pi.y ) ) + pi.z );
	int bab = Perm( Perm( Perm( Inc( pi.x ) ) + pi.y ) + Inc( pi.z ) );
	int bbb = Perm( Perm( Perm( Inc( pi.x ) ) + Inc( pi.y ) ) + Inc( pi.z ) );

	float x1 = lerp( Grad( aaa, pf ), Grad( baa, pf + float3( -1.f, 0.f, 0.f ) ), uvw.x );
	float x2 = lerp( Grad( aba, pf + float3( 0.f, -1.f, 0.f ) ), Grad( bba, pf + float3( -1.f, -1.f, 0.f ) ), uvw.x );
	float y1 = lerp( x1, x2, uvw.y );
	x1 = lerp( Grad( aab, pf + float3( 0.f, 0.f, -1.f ) ), Grad( bab, pf + float3( -1.f, 0.f, -1.f ) ), uvw.x );
	x2 = lerp( Grad( abb, pf + float3( 0.f, -1.f, -1.f ) ), Grad( bbb, pf + float3( -1.f, -1.f, -1.f ) ), uvw.x );
	float y2 = lerp( x1, x2, uvw.y );

	return ( lerp( y1, y2, uvw.z ) + 1.f ) * 0.5f;
}

[numthreads( 8, 8, 8 )]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint3 dims;
	NoiseTex.GetDimensions( dims.x, dims.y, dims.z );

	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float3 p = float3( DTid ) / dims * NoiseScale;
		NoiseTex[DTid] = Perlin( p );
	}
}