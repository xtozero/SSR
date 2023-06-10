#include "VolumetricFogProxy.h"

#include "Components/VolumetricFogComponent.h"

namespace rendercore
{
	const std::array<uint32, 3>& VolumetricFogProxy::FrustumGridSize() const
	{
		return m_frustumGridSize;
	}

	float VolumetricFogProxy::G() const
	{
		return m_g;
	}

	float VolumetricFogProxy::UniformDensity() const
	{
		return m_uniformDensity;
	}

	float VolumetricFogProxy::Intensity() const
	{
		return m_intensity;
	}

	float VolumetricFogProxy::DepthPackExponent() const
	{
		return m_depthPackExponent;
	}

	float VolumetricFogProxy::NearPlaneDist() const
	{
		return m_nearPlaneDist;
	}

	float VolumetricFogProxy::FarPlaneDist() const
	{
		return m_farPlaneDist;
	}

	VolumetricFogProxy::VolumetricFogProxy( const VolumetricFogComponent& component )
		: m_frustumGridSize( component.FrustumGridSize() )
		, m_g( component.G() )
		, m_uniformDensity( component.UniformDensity() )
		, m_intensity( component.Intensity() )
		, m_depthPackExponent( component.DepthPackExponent() )
		, m_nearPlaneDist( component.NearPlaneDist() )
		, m_farPlaneDist( component.FarPlaneDist() )
	{
	}
}
