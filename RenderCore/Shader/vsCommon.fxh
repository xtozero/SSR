cbuffer WORLD : register(b0)
{
	matrix g_worldMatrix : packoffset(c0);
	matrix g_invWorldMatrix : packoffset(c4);
}

cbuffer VEIW_PROJECTION : register(b1)
{
	matrix g_viewMatrix : packoffset(c0);
	matrix g_projectionMatrix : packoffset(c4);
	matrix g_invViewMatrix : packoffset(c8);
	matrix g_invProjectionMatrix : packoffset(c12);
};

cbuffer SHADOW_VIEW_PROJECTION : register(b2)
{
	float g_zBias;
	float3 padding;
	matrix g_lightViewMatrix;
	matrix g_lightProjectionMatrix;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
};
