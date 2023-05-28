#include "VolumetricFog/VolumetricFogCommon.fxh"
#include "Shadow/ShadowCommon.fxh"

RWTexture3D<float4> FrustumVolume : register( u0 );

cbuffer InscatteringParameters : register( b0 )
{
	float AsymmetryParameterG : packoffset( c0.x );
	float UniformDensity : packoffset( c0.y );
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

		float3 lighting = HemisphereUpperColor.rgb * HemisphereUpperColor.a;
		[loop]
		for ( uint i = 0; i < NumLights; ++i )
		{
			ForwardLightData light = GetLight( i );

			float3 lightDirection = { 0.f, 0.f, 0.f };

			if ( length( light.m_direction ) > 0.f )
			{
				lightDirection = -normalize( light.m_direction );
			}
			else
			{
				lightDirection = normalize( light.m_position - worldPosition );
			}

			float phaseFunction = HenyeyGreensteinPhaseFunction( toCamera, lightDirection, AsymmetryParameterG );
			lighting += light.m_diffuse.rgb * light.m_diffuse.a * phaseFunction * UniformDensity;

			FrustumVolume[DTid] = float4( lighting, UniformDensity );
		}
	}
}