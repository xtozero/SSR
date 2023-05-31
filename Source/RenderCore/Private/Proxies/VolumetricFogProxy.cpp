#include "VolumetricFogProxy.h"

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

	VolumetricFogProxy::VolumetricFogProxy( const VolumetricFogComponent& component )
	{
	}
}
