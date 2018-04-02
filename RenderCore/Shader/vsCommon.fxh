cbuffer WORLD : register(b0)
{
	matrix g_worldMatrix : packoffset(c0);
	matrix g_invWorldMatrix : packoffset(c4);
}

cbuffer VEIW_PROJECTION : register(b1)
{
	matrix g_viewMatrix : packoffset(c0);
	matrix g_projectionMatrix : packoffset(c4);
};

cbuffer LIGHT_VEIW_PROJECTION : register(b2)
{
	matrix g_lightViewMatrix : packoffset(c0);
	matrix g_lightProjectionMatrix: packoffset(c4);
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 color : COLOR;
	float2 texcoord : TEXCOORD;
};
