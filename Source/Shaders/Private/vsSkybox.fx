#include "vsCommon.fxh"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 ori_position : POSITION;
};

VS_OUTPUT main( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix worldViewPorjection = mul( g_worldMatrix, g_viewMatrix );
	worldViewPorjection = mul( worldViewPorjection, g_projectionMatrix );

	output.position = mul( float4( input.position, 1.0f ), worldViewPorjection );

	output.ori_position = input.position;

	return output;
}