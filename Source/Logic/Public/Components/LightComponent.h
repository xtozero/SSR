#pragma once

#include "common.h"
#include "Components/SceneComponent.h"
#include "Math/Vector4.h"

class HemisphereLightProxy;
class LightProxy;

enum class ShadowQuility
{
	Low = 0,
	Mid,
	High,
};

class LightComponent : public SceneComponent
{
public:
	using SceneComponent::SceneComponent;

	virtual LightProxy* CreateProxy( ) const = 0;

	void SetDiffuseColor( const ColorF& diffuseColor );
	const ColorF& DiffuseColor( ) const
	{
		return m_diffuse;
	}

	void SetSpecularColor( const ColorF& specularColor );
	const ColorF& SpecularColor( ) const
	{
		return m_specular;
	}

	bool& CastShadow( )
	{
		return m_castShadow;
	}

	bool CastShadow() const
	{
		return m_castShadow;
	}

	ShadowQuility GetShadowQuility( ) const
	{
		return m_shadowQuility;
	}

	LightProxy* m_lightProxy = nullptr;

private:
	ColorF m_diffuse;
	ColorF m_specular;

	bool m_castShadow = false;
	ShadowQuility m_shadowQuility = ShadowQuility::High;
};

class DirectionalLightComponent : public LightComponent
{
public:
	using LightComponent::LightComponent;

	virtual LightProxy* CreateProxy( ) const override;

	void SetDirection( const Vector& direction );
	const Vector& Direction( ) const
	{
		return m_direction;
	}

protected:
	virtual bool ShouldCreateRenderState( ) const override;
	virtual void CreateRenderState( ) override;
	virtual void RemoveRenderState( ) override;

private:
	Vector m_direction;
};

class HemisphereLightComponent : public SceneComponent
{
public:
	using SceneComponent::SceneComponent;

	virtual HemisphereLightProxy* CreateProxy( ) const;

	void SetLowerColor( const ColorF& color )
	{
		m_lowerHemisphereColor = color;
	}

	const ColorF& LowerColor( ) const
	{
		return m_lowerHemisphereColor;
	}

	void SetUpperColor( const ColorF& color )
	{
		m_upperHemisphereColor = color;
	}

	const ColorF& UpperColor( ) const
	{
		return m_upperHemisphereColor;
	}

	Vector UpVector( ) const;

	HemisphereLightProxy*& Proxy( )
	{
		return m_proxy;
	}

protected:
	virtual bool ShouldCreateRenderState( ) const override;
	virtual void CreateRenderState( ) override;
	virtual void RemoveRenderState( ) override;

private:
	HemisphereLightProxy* m_proxy = nullptr;
	ColorF m_lowerHemisphereColor;
	ColorF m_upperHemisphereColor;
};
