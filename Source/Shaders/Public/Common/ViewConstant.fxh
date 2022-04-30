cbuffer SceneViewParameters : register( b1 )
{
	matrix ViewMatrix : packoffset( c0 );
	matrix ProjectionMatrix : packoffset( c4 );
	matrix ViewProjectionMatrix : packoffset( c8 );
	matrix InvViewMatrix : packoffset( c12 );
	matrix InvProjectionMatrix : packoffset( c16 );
	matrix InvViewProjectionMatrix : packoffset( c20 );
	float4 HemisphereUpperColor : packoffset( c24 );
	float4 HemisphereLowerColor : packoffset( c25 );
	float4 HemisphereUpVector : packoffset( c26 );
	float NearPlaneDist : packoffset( c27.x );
	float FarPlaneDist : packoffset( c27.y );
	float ElapsedTime : packoffset( c27.z );
	float TotalTime : packoffset( c27.w );
};