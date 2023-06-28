#pragma once

#include "common.h"
#include "Components/SceneComponent.h"
#include "Math/Vector4.h"

namespace rendercore
{
	class HemisphereLightProxy;
	class LightProxy;
}

enum class ShadowQuility : uint8
{
	Low = 0,
	Mid,
	High,
};

class LightComponent : public SceneComponent
{
	GENERATE_CLASS_TYPE_INFO( LightComponent )

public:
	using SceneComponent::SceneComponent;

	virtual void LoadProperty( const json::Value& json ) override;

	virtual rendercore::LightProxy* CreateProxy() const = 0;
	virtual bool IsUsedAsAtmosphereSunLight() const
	{
		return false;
	}

	void SetDiffuseColor( const ColorF& diffuseColor );
	const ColorF& DiffuseColor() const
	{
		return m_diffuse;
	}

	void SetSpecularColor( const ColorF& specularColor );
	const ColorF& SpecularColor() const
	{
		return m_specular;
	}

	bool& CastShadow()
	{
		return m_castShadow;
	}

	bool CastShadow() const
	{
		return m_castShadow;
	}

	ShadowQuility GetShadowQuility() const
	{
		return m_shadowQuility;
	}

	rendercore::LightProxy* m_lightProxy = nullptr;

private:
	ColorF m_diffuse;
	ColorF m_specular;

	bool m_castShadow = false;
	ShadowQuility m_shadowQuility = ShadowQuility::High;
};

class DirectionalLightComponent : public LightComponent
{
	GENERATE_CLASS_TYPE_INFO( DirectionalLightComponent )

public:
	using LightComponent::LightComponent;

	virtual void LoadProperty( const json::Value& json ) override;

	virtual rendercore::LightProxy* CreateProxy() const override;
	virtual bool IsUsedAsAtmosphereSunLight() const override
	{
		return m_usedAsAtmosphereSunLight;
	}
	void SetUsedAsAtmosphereSunLight( bool on )
	{
		m_usedAsAtmosphereSunLight = on;
	}

	void SetDirection( const Vector& direction );
	const Vector& Direction() const
	{
		return m_direction;
	}

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

private:
	Vector m_direction;
	bool m_usedAsAtmosphereSunLight = false;
};

class HemisphereLightComponent : public SceneComponent
{
	GENERATE_CLASS_TYPE_INFO( HemisphereLightComponent )

public:
	using SceneComponent::SceneComponent;

	virtual void LoadProperty( const json::Value& json ) override;

	virtual rendercore::HemisphereLightProxy* CreateProxy() const;

	void SetLowerColor( const ColorF& color )
	{
		m_lowerHemisphereColor = color;
	}

	const ColorF& LowerColor() const
	{
		return m_lowerHemisphereColor;
	}

	void SetUpperColor( const ColorF& color )
	{
		m_upperHemisphereColor = color;
	}

	const ColorF& UpperColor() const
	{
		return m_upperHemisphereColor;
	}

	Vector UpVector() const;

	rendercore::HemisphereLightProxy*& Proxy()
	{
		return m_proxy;
	}

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

private:
	rendercore::HemisphereLightProxy* m_proxy = nullptr;
	ColorF m_lowerHemisphereColor;
	ColorF m_upperHemisphereColor;
};
