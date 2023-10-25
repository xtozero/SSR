cbuffer SceneViewParameters : register( b1 )
{
	matrix ViewMatrix : packoffset( c0 );
	matrix ProjectionMatrix : packoffset( c4 );
	matrix ViewProjectionMatrix : packoffset( c8 );
	matrix InvViewMatrix : packoffset( c12 );
	matrix InvProjectionMatrix : packoffset( c16 );
	matrix InvViewProjectionMatrix : packoffset( c20 );
	matrix PrevViewMatrix : packoffset( c24 );
	matrix PrevProjectionMatrix : packoffset( c28 );
	matrix PrevViewProjectionMatrix : packoffset( c32 );
	float NearPlaneDist : packoffset( c36.x );
	float FarPlaneDist : packoffset( c36.y );
	float ElapsedTime : packoffset( c36.z );
	float TotalTime : packoffset( c36.w );
	float3 CameraPos : packoffset( c37.x );
	uint FrameCount : packoffset( c37.w );
	float2 ViewportDimensions : packoffset( c38 );
};