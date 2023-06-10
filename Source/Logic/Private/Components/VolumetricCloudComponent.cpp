#include "Components/VolumetricCloudComponent.h"

#include "Json/Json.hpp"
#include "Proxies/VolumetricCloudProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

void VolumetricCloudComponent::LoadProperty( const JSON::Value& json )
{
	Super::LoadProperty( json );

	if ( const JSON::Value* pEarthRadius = json.Find( "EarthRadius" ) )
	{
		SetEarthRadius( static_cast<float>( pEarthRadius->AsReal() ) );
	}

	if ( const JSON::Value* pInnerRadius = json.Find( "InnerRadius" ) )
	{
		SetInnerRadius( static_cast<float>( pInnerRadius->AsReal() ) );
	}

	if ( const JSON::Value* pOuterRadius = json.Find( "OuterRadius" ) )
	{
		SetOuterRadius( static_cast<float>( pOuterRadius->AsReal() ) );
	}

	if ( const JSON::Value* pLightAbsorption = json.Find( "LightAbsorption" ) )
	{
		SetLightAbsorption( static_cast<float>( pLightAbsorption->AsReal() ) );
	}

	if ( const JSON::Value* pDensityScale = json.Find( "DensityScale" ) )
	{
		SetDensityScale( static_cast<float>( pDensityScale->AsReal() ) );
	}

	if ( const JSON::Value* pCloudColor = json.Find( "CloudColor" ) )
	{
		const JSON::Value& cloudColor = *pCloudColor;

		if ( cloudColor.Size() == 3 )
		{
			SetCloudColor(
				ColorF( static_cast<float>( cloudColor[0].AsReal() )
					, static_cast<float>( cloudColor[1].AsReal() )
					, static_cast<float>( cloudColor[2].AsReal() )
					, 1.f ) );
		}
	}

	if ( const JSON::Value* pCrispiness = json.Find( "Crispiness" ) )
	{
		SetCrispiness( static_cast<float>( pCrispiness->AsReal() ) );
	}

	if ( const JSON::Value* pCurliness = json.Find( "Curliness" ) )
	{
		SetCurliness( static_cast<float>( pCurliness->AsReal() ) );
	}

	if ( const JSON::Value* pDensityFactor = json.Find( "DensityFactor" ) )
	{
		SetDensityFactor( static_cast<float>( pDensityFactor->AsReal() ) );
	}
}

BoxSphereBounds VolumetricCloudComponent::CalcBounds( const Matrix& transform )
{
	BoxSphereBounds bounds( Vector::ZeroVector, Vector::OneVector, 1.73205f );
	return bounds.TransformBy( transform );
}

rendercore::VolumetricCloudProxy* VolumetricCloudComponent::CreateProxy()
{
	return new rendercore::VolumetricCloudProxy( *this );
}

void VolumetricCloudComponent::SetEarthRadius( float earthRadius )
{
	m_earthRadius = earthRadius;
}

void VolumetricCloudComponent::SetInnerRadius( float innerRadius )
{
	m_innerRadius = innerRadius;
}

void VolumetricCloudComponent::SetOuterRadius( float outerRadius )
{
	m_outerRadius = outerRadius;
}

void VolumetricCloudComponent::SetLightAbsorption( float lightAbsorption )
{
	m_lightAbsorption = lightAbsorption;
}

void VolumetricCloudComponent::SetDensityScale( float densityScale )
{
	m_densityScale = densityScale;
}

void VolumetricCloudComponent::SetCloudColor( const ColorF& cloudColor )
{
	m_cloudColor = cloudColor;
}

void VolumetricCloudComponent::SetCrispiness( float crispiness )
{
	m_crispiness = crispiness;
}

void VolumetricCloudComponent::SetCurliness( float curliness )
{
	m_curliness = curliness;
}

void VolumetricCloudComponent::SetDensityFactor( float densityFactor )
{
	m_densityFactor = densityFactor;
}

bool VolumetricCloudComponent::ShouldCreateRenderState() const
{
	return true;
}

void VolumetricCloudComponent::CreateRenderState()
{
	Super::CreateRenderState();
	m_pWorld->Scene()->AddVolumetricCloud( this );
}

void VolumetricCloudComponent::RemoveRenderState()
{
	Super::RemoveRenderState();
	m_pWorld->Scene()->RemoveVolumetricCloud( this );
}
