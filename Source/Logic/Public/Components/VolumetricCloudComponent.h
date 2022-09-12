#pragma once

#include "Math/Vector4.h"
#include "SceneComponent.h"

namespace rendercore
{
	class VolumetricCloudProxy;
}

class VolumetricCloudComponent : public SceneComponent
{
	GENERATE_CLASS_TYPE_INFO( VolumetricCloudComponent )

public:
	using SceneComponent::SceneComponent;

	virtual void LoadProperty( const JSON::Value& json ) override;

	virtual BoxSphereBounds CalcBounds( const Matrix& transform ) override;

	rendercore::VolumetricCloudProxy*& Proxy()
	{
		return m_proxy;
	}

	virtual rendercore::VolumetricCloudProxy* CreateProxy();

	float EarthRadius() const
	{
		return m_earthRadius;
	}

	void SetEarthRadius( float earthRadius );

	float InnerRadius() const
	{
		return m_innerRadius;
	}

	void SetInnerRadius( float innerRadius );

	float OuterRadius() const
	{
		return m_outerRadius;
	}

	void SetOuterRadius( float outerRadius );

	float LightAbsorption() const
	{
		return m_lightAbsorption;
	}

	void SetLightAbsorption( float lightAbsorption );

	float DensityScale() const
	{
		return m_densityScale;
	}

	void SetDensityScale( float densityScale );

	const ColorF& CloudColor() const
	{
		return m_cloudColor;
	}

	void SetCloudColor( const ColorF& cloudColor );

	float Crispiness() const
	{
		return m_crispiness;
	}

	void SetCrispiness( float crispiness );

	float Curliness() const
	{
		return m_curliness;
	}

	void SetCurliness( float curliness );

	float DensityFactor() const
	{
		return m_densityFactor;
	}

	void SetDensityFactor( float densityFactor );

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

private:
	rendercore::VolumetricCloudProxy* m_proxy = nullptr;
	float m_earthRadius = 0.f;
	float m_innerRadius = 0.f;
	float m_outerRadius = 0.f;
	float m_lightAbsorption = 0.0035f;
	float m_densityScale = 0.45f;
	ColorF m_cloudColor = { 0.38235f, 0.41176f, 0.47059f, 1.f };
	float m_crispiness = 40.f;
	float m_curliness = 0.1f;
	float m_densityFactor = 0.2f;
};
