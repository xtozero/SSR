#include "GameObject/VolumetricCloud.h"

#include "Components/VolumetricCloudComponent.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

DECLARE_GAME_OBJECT( volumetric_cloud, VolumetricCloud )

void VolumetricCloud::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	if ( const JSON::Value* pEarthRadius = json.Find( "EarthRadius" ) )
	{
		m_volumetricCloudComponent->SetEarthRadius( static_cast<float>( pEarthRadius->AsReal() ) );
	}

	if ( const JSON::Value* pInnerRadius = json.Find( "InnerRadius" ) )
	{
		m_volumetricCloudComponent->SetInnerRadius( static_cast<float>( pInnerRadius->AsReal() ) );
	}

	if ( const JSON::Value* pOuterRadius = json.Find( "OuterRadius" ) )
	{
		m_volumetricCloudComponent->SetOuterRadius( static_cast<float>( pOuterRadius->AsReal() ) );
	}

	if ( const JSON::Value* pLightAbsorption = json.Find( "LightAbsorption" ) )
	{
		m_volumetricCloudComponent->SetLightAbsorption( static_cast<float>( pLightAbsorption->AsReal() ) );
	}

	if ( const JSON::Value* pDensityScale = json.Find( "DensityScale" ) )
	{
		m_volumetricCloudComponent->SetDensityScale( static_cast<float>( pDensityScale->AsReal() ) );
	}

	if ( const JSON::Value* pCloudColor = json.Find( "CloudColor" ) )
	{
		const JSON::Value& cloudColor = *pCloudColor;

		if ( cloudColor.Size() == 3 )
		{
			m_volumetricCloudComponent->SetCloudColor( {
				static_cast<float>( cloudColor[0].AsReal() ),
				static_cast<float>( cloudColor[1].AsReal() ),
				static_cast<float>( cloudColor[2].AsReal() ),
				1.f }
			);
		}
	}

	if ( const JSON::Value* pCrispiness = json.Find( "Crispiness" ) )
	{
		m_volumetricCloudComponent->SetCrispiness( static_cast<float>( pCrispiness->AsReal() ) );
	}

	if ( const JSON::Value* pCurliness = json.Find( "Curliness" ) )
	{
		m_volumetricCloudComponent->SetCurliness( static_cast<float>( pCurliness->AsReal() ) );
	}

	if ( const JSON::Value* pDensityFactor = json.Find( "DensityFactor" ) )
	{
		m_volumetricCloudComponent->SetDensityFactor( static_cast<float>( pDensityFactor->AsReal() ) );
	}

	Super::LoadProperty( gameLogic, json );
}

VolumetricCloud::VolumetricCloud()
{
	m_volumetricCloudComponent = CreateComponent<VolumetricCloudComponent>( *this );
	m_rootComponent = m_volumetricCloudComponent;
}