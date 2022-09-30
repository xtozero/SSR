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
	float4 HemisphereUpperColor : packoffset( c36 );
	float4 HemisphereLowerColor : packoffset( c37 );
	float4 HemisphereUpVector : packoffset( c38 );
	float NearPlaneDist : packoffset( c39.x );
	float FarPlaneDist : packoffset( c39.y );
	float ElapsedTime : packoffset( c39.z );
	float TotalTime : packoffset( c39.w );
	uint FrameCount : packoffset( c40.x );
	float2 ViewportDimensions : packoffset( c40.y );
};