#include "stdafx.h"
#include "LightProxy.h"

#include "Components/LightComponent.h"
#include "Physics/Frustum.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/ShadowInfo.h"

LightProxy::LightProxy( const LightComponent& component ) :
	m_diffuse( component.DiffuseColor( ) ),
	m_specular( component.SpecularColor( ) ),
	m_castShadow( component.CastShadow( ) ),
	m_usedAsAtmosphereSunLight( component.IsUsedAsAtmosphereSunLight() )
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
		Vector::ZeroVector,
		-m_direction * std::numeric_limits<float>::max( ),
		m_diffuse,
		m_specular
	};

	return lightProperty;
}

bool DirectionalLightProxy::AffactsBounds( [[maybe_unused]] const BoxSphereBounds& bounds ) const
{
	return true;
}

DirectionalLightProxy::DirectionalLightProxy( const DirectionalLightComponent& component ) :
	LightProxy( component ),
	m_direction( component.Direction( ) )
{
}

HemisphereLightProxy::HemisphereLightProxy( const ColorF& lowerColor, const ColorF& upperColor, const Vector& upVector ) :
	m_lowerHemisphereColor( lowerColor ),
	m_upperHemisphereColor( upperColor ),
	m_upVector( upVector )
{
}
