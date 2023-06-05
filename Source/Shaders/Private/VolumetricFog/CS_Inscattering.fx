#include "VolumetricFog/VolumetricFogCommon.fxh"
#include "Shadow/ShadowCommon.fxh"

RWTexture3D<float4> FrustumVolume : register( u0 );

cbuffer InscatteringParameters : register( b0 )
{
	float AsymmetryParameterG : packoffset( c0.x );
	float UniformDensity : packoffset( c0.y );
	float Intensity : packoffset( c0.z );
}

float Visibility( float3 worldPos, float3 biasDir )
{
	float3 viewPos = mul( float4( worldPos, 1.f ), ViewMatrix ).xyz;
	return CalcShadowVisibility( worldPos, viewPos, biasDir );
}

[numthreads( 8, 8, 8 )]
void main( uint3 DTid : SV_DispatchThreadId )
{
	uint3 dims;
	FrustumVolume.GetDimensions( dims.x, dims.y, dims.z );
	
	if ( DTid.x < dims.x && DTid.y < dims.y && DTid.z < dims.z )
	{
		float3 worldPosition = ConvertThreadIdToWorldPosition( DTid, dims );
		float3 toCamera = normalize( CameraPos - worldPosition );

		float ndcZ = ConvertThreadIdToNdc( DTid, dims ).z;
		float tickness = SliceTickness( ndcZ, dims.z );

		float density = UniformDensity * tickness;

		float3 lighting = HemisphereUpperColor.rgb * HemisphereUpperColor.a;
		[loop]
		for ( uint i = 0; i < NumLights; ++i )
		{
			ForwardLightData light = GetLight( i );

			float3 lightDirection = { 0.f, 0.f, 0.f };

			if ( length( light.m_direction ) > 0.f )
			{
				lightDirection = normalize( light.m_direction );
			}
			else
			{
				lightDirection = normalize( worldPosition - light.m_position );
			}

			float3 toLight = -lightDirection;
			float visibility = Visibility( worldPosition, toLight );
			float phaseFunction = HenyeyGreensteinPhaseFunction( toCamera, lightDirection, AsymmetryParameterG );
			lighting += visibility * light.m_diffuse.rgb * light.m_diffuse.a * phaseFunction;

			FrustumVolume[DTid] = float4( lighting * Intensity * density, density );
		}
	}
}