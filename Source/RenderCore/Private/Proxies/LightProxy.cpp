#include "LightProxy.h"

#include "Components/LightComponent.h"
#include "Physics/Frustum.h"
#include "Physics/ICollider.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/ShadowInfo.h"

namespace
{
	// from https://wiki.ogre3d.org/Light+Attenuation+Shortcut
	Vector DistanceAttenuation( float range )
	{
		return Vector( 1.0f, 4.5f / range, 75.0f / ( range * range ) );
	}
}

namespace rendercore
{
	LightProxy::LightProxy( const logic::LightComponent& component ) 
		: m_diffuse( component.DiffuseColor() )
		, m_specular( component.SpecularColor() )
		, m_castShadow( component.CastShadow() )
		, m_usedAsAtmosphereSunLight( component.IsUsedAsAtmosphereSunLight() )
	{
	}

	LightProperty DirectionalLightProxy::GetLightProperty() const
	{
		LightProperty lightProperty = {
			.m_type = LightType::Directional,
			.m_theta = 0.f,
			.m_phi = 0.f,
			.m_direction = m_direction,
			.m_range = std::numeric_limits<float>::max(),
			.m_fallOff = 0.f,
			.m_attenuation = Vector( 1.f, 0.f, 0.f ),
			.m_position = Vector::ZeroVector,
			.m_diffuse = m_diffuse,
			.m_specular = m_specular
		};

		return lightProperty;
	}

	Vector DirectionalLightProxy::GetDirection() const
	{
		return m_direction;
	}

	float DirectionalLightProxy::GetRange() const
	{
		assert( false && "DirectionalLightProxy - Unsurpported function" );
		return 0.0f;
	}

	Vector DirectionalLightProxy::GetPosition() const
	{
		assert( false && "DirectionalLightProxy - Unsurpported function" );
		return Vector::ZeroVector;
	}

	bool DirectionalLightProxy::AffactsBounds( [[maybe_unused]] const BoxSphereBounds& bounds ) const
	{
		return true;
	}

	DirectionalLightProxy::DirectionalLightProxy( const logic::DirectionalLightComponent& component ) :
		LightProxy( component ),
		m_direction( component.Direction() )
	{
	}

	LightProperty PointLightProxy::GetLightProperty() const
	{
		LightProperty lightProperty = {
			.m_type = LightType::Point,
			.m_theta = 0.f,
			.m_phi = 0.f,
			.m_direction = Vector::ZeroVector,
			.m_range = m_range,
			.m_fallOff = 0.f,
			.m_attenuation = m_attenuation,
			.m_position = m_position,
			.m_diffuse = m_diffuse,
			.m_specular = m_specular
		};

		return lightProperty;
	}

	Vector PointLightProxy::GetDirection() const
	{
		assert( false && "PointLightProxy - Unsurpported function" );
		return Vector::ZeroVector;
	}

	float PointLightProxy::GetRange() const
	{
		return m_range;
	}

	Vector PointLightProxy::GetPosition() const
	{
		return m_position;
	}

	bool PointLightProxy::AffactsBounds( const BoxSphereBounds& bounds ) const
	{
		BoxSphereBounds lightBounds( m_position, Vector( m_range, m_range, m_range ), m_range );

		return lightBounds.Overlapped( bounds ) != CollisionResult::Outside;
	}

	PointLightProxy::PointLightProxy( const logic::PointLightComponent& component ) 
		: LightProxy( component )
		, m_range( component.Range() )
		, m_attenuation( DistanceAttenuation( m_range ) )
		, m_position( component.GetPosition() )
	{
	}

	LightProperty SpotLightProxy::GetLightProperty() const
	{
		LightProperty lightProperty = {
			.m_type = LightType::Spot,
			.m_theta = m_theta,
			.m_phi = m_phi,
			.m_direction = m_direction,
			.m_range = m_range,
			.m_fallOff = 0.f,
			.m_attenuation = m_attenuation,
			.m_position = m_position,
			.m_diffuse = m_diffuse,
			.m_specular = m_specular
		};

		return lightProperty;
	}

	Vector SpotLightProxy::GetDirection() const
	{
		return m_direction;
	}

	float SpotLightProxy::GetRange() const
	{
		return m_range;
	}

	Vector SpotLightProxy::GetPosition() const
	{
		return m_position;
	}

	bool SpotLightProxy::AffactsBounds( const BoxSphereBounds& bounds ) const
	{
		return false;
	}

	SpotLightProxy::SpotLightProxy( const logic::SpotLightComponent& component )
		: LightProxy( component )
		, m_theta( std::cos( component.InnerAngle() ) )
		, m_phi( std::cos( component.OuterAngle() ) )
		, m_direction( component.Direction() )
		, m_range( component.Range() )
		, m_attenuation( DistanceAttenuation( m_range ) )
		, m_position( component.GetPosition() )
	{
	}

	HemisphereLightProxy::HemisphereLightProxy( const ColorF& lowerColor, const ColorF& upperColor, const Vector& upVector ) :
		m_lowerHemisphereColor( lowerColor ),
		m_upperHemisphereColor( upperColor ),
		m_upVector( upVector )
	{
	}
}
