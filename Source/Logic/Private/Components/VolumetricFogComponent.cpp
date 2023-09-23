#include "Components/VolumetricFogComponent.h"

#include "Json/json.hpp"
#include "Proxies/VolumetricFogProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

namespace logic
{
	void VolumetricFogComponent::LoadProperty( const json::Value& json )
	{
		Super::LoadProperty( json );

		if ( const json::Value* pFrustumGridSize = json.Find( "FrustumGridSize" ) )
		{
			const json::Value& frustumGridSize = *pFrustumGridSize;

			if ( frustumGridSize.Size() == 3 )
			{
				for ( int32 i = 0; i < frustumGridSize.Size(); ++i )
				{
					m_frustumGridSize[i] = frustumGridSize[i].AsInt();
				}
			}
		}

		if ( const json::Value* pG = json.Find( "G" ) )
		{
			m_g = static_cast<float>( pG->AsReal() );
		}

		if ( const json::Value* pUniformDensity = json.Find( "UniformDensity" ) )
		{
			m_uniformDensity = static_cast<float>( pUniformDensity->AsReal() );
		}

		if ( const json::Value* pIntensity = json.Find( "Intensity" ) )
		{
			m_intensity = static_cast<float>( pIntensity->AsReal() );
		}

		if ( const json::Value* pDepthPackExponent = json.Find( "DepthPackExponent" ) )
		{
			m_depthPackExponent = static_cast<float>( pDepthPackExponent->AsReal() );
		}

		if ( const json::Value* pNearPlaneDist = json.Find( "NearPlaneDist" ) )
		{
			m_nearPlaneDist = static_cast<float>( pNearPlaneDist->AsReal() );
		}

		if ( const json::Value* pFarPlaneDist = json.Find( "FarPlaneDist" ) )
		{
			m_farPlaneDist = static_cast<float>( pFarPlaneDist->AsReal() );
		}

		if ( const json::Value* pShadowBias = json.Find( "ShadowBias" ) )
		{
			m_shadowBias = static_cast<float>( pShadowBias->AsReal() );
		}
	}

	rendercore::VolumetricFogProxy* VolumetricFogComponent::CreateProxy()
	{
		return new rendercore::VolumetricFogProxy( *this );
	}

	bool VolumetricFogComponent::ShouldCreateRenderState() const
	{
		return true;
	}

	void VolumetricFogComponent::CreateRenderState()
	{
		Super::CreateRenderState();

		m_pWorld->Scene()->AddVolumetricFog( this );
	}

	void VolumetricFogComponent::RemoveRenderState()
	{
		Super::RemoveRenderState();

		m_pWorld->Scene()->RemoveVolumetricFog( this );
	}
}
