#include "stdafx.h"
#include "GameObject/Light.h"

#include "Components/LightComponent.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

#include <cassert>

namespace logic
{
	void Light::SetDiffuseColor( const ColorF& diffuseColor )
	{
		GetLightComponent().SetDiffuseColor( diffuseColor );
	}

	void Light::SetSpecularColor( const ColorF& specularColor )
	{
		GetLightComponent().SetSpecularColor( specularColor );
	}

	void Light::SetCastShadow( bool castShadow )
	{
		GetLightComponent().CastShadow() = castShadow;
	}

	LightComponent& Light::GetLightComponent()
	{
		if ( m_component == nullptr )
		{
			m_component = GetComponent<LightComponent>();
		}

		return *m_component;
	}

	DECLARE_GAME_OBJECT( directional_light, DirectionalLight );

	const LightType DirectionalLight::GetType() const
	{
		return LightType::Directional;
	}

	const Vector& DirectionalLight::Direction() const
	{
		if ( m_directionalLightComponent )
		{
			return m_directionalLightComponent->Direction();
		}

		return Vector::ZeroVector;
	}

	DirectionalLight::DirectionalLight()
	{
		m_directionalLightComponent = CreateComponent<DirectionalLightComponent>( *this, "DirectionalLightComponent" );
		SetRootComponent( m_directionalLightComponent );
	}

	DECLARE_GAME_OBJECT( point_light, PointLight );

	const LightType PointLight::GetType() const
	{
		return LightType::Point;
	}

	PointLight::PointLight()
	{
		m_pointLightComponent = CreateComponent<PointLightComponent>( *this, "PointLightComponent" );
		SetRootComponent( m_pointLightComponent );
	}

	DECLARE_GAME_OBJECT( spot_light, SpotLight );

	const LightType SpotLight::GetType() const
	{
		return LightType::Spot;
	}

	const Vector& SpotLight::Direction() const
	{
		if ( m_spotLightComponent )
		{
			return m_spotLightComponent->Direction();
		}

		return Vector::ZeroVector;
	}

	SpotLight::SpotLight()
	{
		m_spotLightComponent = CreateComponent<SpotLightComponent>( *this, "SpotLightComponent" );
		SetRootComponent( m_spotLightComponent );
	}

	DECLARE_GAME_OBJECT( hemisphere_light, HemisphereLight );

	void HemisphereLight::SetLowerColor( const ColorF& color )
	{
		if ( m_hemisphereLightComponent )
		{
			m_hemisphereLightComponent->SetLowerColor( color );
		}
	}

	void HemisphereLight::SetUpperColor( const ColorF& color )
	{
		if ( m_hemisphereLightComponent )
		{
			m_hemisphereLightComponent->SetUpperColor( color );
		}
	}

	HemisphereLight::HemisphereLight()
	{
		m_hemisphereLightComponent = CreateComponent<HemisphereLightComponent>( *this, "HemisphereLightComponent" );
		SetRootComponent( m_hemisphereLightComponent );
	}
}
