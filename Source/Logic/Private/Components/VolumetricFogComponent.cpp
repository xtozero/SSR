#include "Components/VolumetricFogComponent.h"

#include "Json/json.hpp"
#include "Proxies/VolumetricFogProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

void VolumetricFogComponent::LoadProperty( const JSON::Value& json )
{
	Super::LoadProperty( json );

	if ( const JSON::Value* pFrustumGridSize = json.Find( "FrustumGridSize" ) )
	{
		const JSON::Value& frustumGridSize = *pFrustumGridSize;

		if ( frustumGridSize.Size() == 3 )
		{
			for ( int32 i = 0; i < frustumGridSize.Size(); ++i )
			{
				m_frustumGridSize[i] = frustumGridSize[i].AsInt();
			}
		}
	}

	if ( const JSON::Value* pG = json.Find( "G" ) )
	{
		m_g = static_cast<float>( pG->AsReal() );
	}

	if ( const JSON::Value* pUniformDensity = json.Find( "UniformDensity" ) )
	{
		m_uniformDensity = static_cast<float>( pUniformDensity->AsReal() );
	}

	if ( const JSON::Value* pIntensity = json.Find( "Intensity" ) )
	{
		m_intensity = static_cast<float>( pIntensity->AsReal() );
	}

	if ( const JSON::Value* pDepthPackExponent = json.Find( "DepthPackExponent" ) )
	{
		m_depthPackExponent = static_cast<float>( pDepthPackExponent->AsReal() );
	}

	if ( const JSON::Value* pNearPlaneDist = json.Find( "NearPlaneDist" ) )
	{
		m_nearPlaneDist = static_cast<float>( pNearPlaneDist->AsReal() );
	}

	if ( const JSON::Value* pFarPlaneDist = json.Find( "FarPlaneDist" ) )
	{
		m_farPlaneDist = static_cast<float>( pFarPlaneDist->AsReal() );
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
