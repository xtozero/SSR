#include "Components/VolumetricCloudComponent.h"

#include "Proxies/VolumetricCloudProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

BoxSphereBounds VolumetricCloudComponent::CalcBounds( const Matrix& transform )
{
	BoxSphereBounds bounds( Vector::ZeroVector, Vector::OneVector, 1.73205f );
	return bounds.TransformBy( transform );
}

VolumetricCloudProxy* VolumetricCloudComponent::CreateProxy()
{
	return new VolumetricCloudProxy( *this );
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
