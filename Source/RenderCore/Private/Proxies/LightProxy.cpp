#include "stdafx.h"
#include "LightProxy.h"

#include "Components/LightComponent.h"

LightProxy::LightProxy( const LightComponent& component ) :
	m_diffuse( component.DiffuseColor( ) ),
	m_specular( component.SpecularColor( ) )
{
}

LightProperty DirectionalLightProxy::GetLightProperty( ) const
{
	LightProperty lightProperty = {
		LIGHT_TYPE::DIRECTINAL_LIGHT,
		0.f,
		0.f,
		m_direction,
		std::numeric_limits<float>::max( ),
		0.f,
		CXMFLOAT3( ),
		-m_direction * std::numeric_limits<float>::max( ),
		m_diffuse,
		m_specular
	};

	return lightProperty;
}

DirectionalLightProxy::DirectionalLightProxy( const DirectionalLightComponent& component ) :
	LightProxy( component ),
	m_direction( component.Direction( ) )
{
}

HemisphereLightProxy::HemisphereLightProxy( const CXMFLOAT4& lowerColor, const CXMFLOAT4& upperColor, const CXMFLOAT3& upVector ) :
	m_lowerHemisphereColor( lowerColor ),
	m_upperHemisphereColor( upperColor ),
	m_upVector( upVector )
{
}
