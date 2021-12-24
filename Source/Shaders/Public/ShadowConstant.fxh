#define MAX_CASCADED_NUM 2

struct CascadeConstant
{
	float m_zFar;
	float3 padding;
};

cbuffer ShadowDepthPassParameters : register( b0 )
{
	float4 g_lightPosOrDir;
	float g_slopeBiasScale;
	float g_constantBias;
	float2 m_padding1;

	CascadeConstant g_cascadeConstant[MAX_CASCADED_NUM];
	matrix g_shadowViewProjection[MAX_CASCADED_NUM];
};