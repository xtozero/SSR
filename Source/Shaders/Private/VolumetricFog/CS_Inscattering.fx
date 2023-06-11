#include "Constants.fxh"
#include "Shadow/ShadowCommon.fxh"
#include "VolumetricFog/VolumetricFogCommon.fxh"

RWTexture3D<float4> FrustumVolume : register( u0 );

Texture3D HistoryVolume : register( t0 );
SamplerState HistorySampler : register( s0 );

cbuffer InscatteringParameters : register( b0 )
{
	float AsymmetryParameterG : packoffset( c0.x );
	float UniformDensity : packoffset( c0.y );
	float Intensity : packoffset( c0.z );
	float TemporalAccum : packoffset( c0.w );
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
		float3 jitter = HALTON_SEQUENCE[( FrameCount + DTid.x + DTid.y * 2 ) % MAX_HALTON_SEQUENCE];
		jitter.xy -= 0.5f;

		float3 ndc = ConvertThreadIdToNdc( DTid, dims, jitter );
		float tickness = SliceTickness( ndc.z, dims.z );

		float depth = ConvertNdcZToDepth( ndc.z );
		float3 worldPosition = ConvertToWorldPosition( ndc, depth );
		float3 toCamera = normalize( CameraPos - worldPosition );

		float density = UniformDensity * tickness;

		float4 curScattering;

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

			curScattering = float4( lighting * Intensity * density, density );
		}

		if ( TemporalAccum > 0.f )
		{
			float3 worldPosWithoutJitter = ConvertThreadIdToWorldPosition( DTid, dims );
			float3 prevFrameUV = ConvertWorldPositionToUV( worldPosWithoutJitter, PrevViewProjectionMatrix );
			if ( all( prevFrameUV <= ( float3 )1.f ) && all( prevFrameUV >= ( float3 )0.f ) )
			{
				float4 prevScattering = HistoryVolume.SampleLevel( HistorySampler, prevFrameUV, 0 );
				curScattering = lerp( prevScattering, curScattering, 0.05f );
			}
		}

		FrustumVolume[DTid] = curScattering;
	}
}