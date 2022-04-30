#include "Common/PsCommon.fxh"
#include "Common/TexCommon.fxh"

Texture2D srcTex : register(t1);

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 main( PS_INPUT input ) : SV_TARGET
{
	float GaussianKernal[7];
	
	GaussianKernal[0] = 0.071303;
	GaussianKernal[1] = 0.131514;
	GaussianKernal[2] = 0.189879;
	GaussianKernal[3] = 0.214607;
	GaussianKernal[4] = 0.189879;
	GaussianKernal[5] = 0.131514;
	GaussianKernal[6] = 0.071303;

	float2 offset1 = float2( 0.0f, 1.f ) * g_invTargetSize.y;
	float2 offset2 = float2( 0.0f, 2.f ) * g_invTargetSize.y;
	float2 offset3 = float2( 0.0f, 3.f ) * g_invTargetSize.y;

	float2 position[7];

	position[0] = input.texcoord - offset3;
	position[1] = input.texcoord - offset2;
	position[2] = input.texcoord - offset1;
	position[3] = input.texcoord;
	position[4] = input.texcoord + offset1;
	position[5] = input.texcoord + offset2;
	position[6] = input.texcoord + offset3;

	float4 result = float4(0.f, 0.f, 0.f, 0.f);

	for ( int i = 0; i < 7; ++i )
	{
		result += srcTex.Sample( baseSampler, position[i] ) * GaussianKernal[i];
	}

	return result;
}