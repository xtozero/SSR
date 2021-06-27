cbuffer VEIW_PROJECTION : register(b1)
{
	matrix g_viewMatrix : packoffset(c0);
	matrix g_projectionMatrix : packoffset(c4);
	matrix g_viewProjectionMatrix : packoffset(c8);
	matrix g_invViewMatrix : packoffset(c12);
	matrix g_invProjectionMatrix : packoffset(c16);
	matrix g_invViewProjectionMatrix : packoffset(c20);
};