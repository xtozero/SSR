#pragma once

#include "Components/Component.h"

#include <array>

namespace rendercore
{
	class VolumetricFogProxy;
}

class VolumetricFogComponent : public Component
{
	GENERATE_CLASS_TYPE_INFO( VolumetricFogComponent )

public:
	using Component::Component;

	virtual void LoadProperty( const JSON::Value& json ) override;

	rendercore::VolumetricFogProxy*& Proxy()
	{
		return m_proxy;
	}

	virtual rendercore::VolumetricFogProxy* CreateProxy();

	const std::array<uint32, 3>& FrustumGridSize() const
	{
		return m_frustumGridSize;
	}

	float G() const
	{
		return m_g;
	}

	float UniformDensity() const
	{
		return m_uniformDensity;
	}

	float Intensity() const
	{
		return m_intensity;
	}

	float DepthPackExponent() const
	{
		return m_depthPackExponent;
	}

	float NearPlaneDist() const
	{
		return m_nearPlaneDist;
	}

	float FarPlaneDist() const
	{
		return m_farPlaneDist;
	}

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

private:
	rendercore::VolumetricFogProxy* m_proxy = nullptr;

	std::array<uint32, 3> m_frustumGridSize = { 160, 90, 128 };
	float m_g = 0.7f;
	float m_uniformDensity = 5.f;
	float m_intensity = 50.f;
	float m_depthPackExponent = 2.f;
	float m_nearPlaneDist = 1.f;
	float m_farPlaneDist = 300.f;
};