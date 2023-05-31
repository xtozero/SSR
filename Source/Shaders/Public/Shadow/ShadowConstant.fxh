#define MAX_CASCADED_NUM 2

struct CascadeConstant
{
	float m_zFar;
	float3 padding;
};

cbuffer ShadowDepthPassParameters : register( b4 )
{
	float4 LightPosOrDir : packoffset( c0 );
	float SlopeBiasScale : packoffset( c1 );
	float ConstantBias : packoffset( c1.y );

	CascadeConstant CascadeConstants[MAX_CASCADED_NUM] : packoffset( c2 );
	matrix ShadowViewProjection[6]: packoffset( c4 );
};