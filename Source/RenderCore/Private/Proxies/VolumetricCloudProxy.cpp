#include "VolumetricCloudProxy.h"

#include "Components/VolumetricCloudComponent.h"

VolumetricCloudProxy::VolumetricCloudProxy( const VolumetricCloudComponent& component )
{
	m_earthRadius = component.EarthRadius();
	m_innerRadius = component.InnerRadius();
	m_outerRadius = component.OuterRadius();
	m_lightAbsorption = component.LightAbsorption();
	m_densityScale = component.DensityScale();
	m_cloudColor = component.CloudColor();
	m_crispiness = component.Crispiness();
	m_curliness = component.Curliness();
	m_densityFactor = component.DensityFactor();
}

float VolumetricCloudProxy::EarthRadius() const
{
	return m_earthRadius;
}

float VolumetricCloudProxy::InnerRadius() const
{
	return m_innerRadius;
}

float VolumetricCloudProxy::OuterRadius() const
{
	return m_outerRadius;
}

float VolumetricCloudProxy::LightAbsorption() const
{
	return m_lightAbsorption;
}

float VolumetricCloudProxy::DensityScale() const
{
	return m_densityScale;
}

const ColorF& VolumetricCloudProxy::CloudColor() const
{
	return m_cloudColor;
}

float VolumetricCloudProxy::Crispiness() const
{
	return m_crispiness;
}

float VolumetricCloudProxy::Curliness() const
{
	return m_curliness;
}

float VolumetricCloudProxy::DensityFactor() const
{
	return m_densityFactor;
}
