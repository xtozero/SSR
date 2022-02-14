cbuffer VEIW_PROJECTION : register( b1 )
{
	matrix g_viewMatrix : packoffset(c0);
	matrix g_projectionMatrix : packoffset(c4);
	matrix g_viewProjectionMatrix : packoffset(c8);
	matrix g_invViewMatrix : packoffset(c12);
	matrix g_invProjectionMatrix : packoffset(c16);
	matrix g_invViewProjectionMatrix : packoffset(c20);
	float4 g_hemisphereUpperColor : packoffset(c24);
	float4 g_hemisphereLowerColor : packoffset(c25);
	float3 g_hemisphereUpVector : packoffset(c26);
	float g_padding1 : packoffset(c26.w);
	float g_nearPlaneDist : packoffset(c27.x);
	float g_FarPlaneDist : packoffset(c27.y);
	float ElapsedTime : packoffset(c27.z);
	float TotalTime : packoffset(c27.w);
};