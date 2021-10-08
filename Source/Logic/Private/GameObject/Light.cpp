#include "stdafx.h"
#include "GameObject/Light.h"

#include "Components/LightComponent.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

#include <cassert>

using namespace DirectX;

//const bool CLight::IsOn( ) const
//{
//	if ( m_property )
//	{
//		return m_property->m_isOn;
//	}
//
//	return false;
//}

//void CLight::SetOnOff( const bool on )
//{
//	if ( m_property )
//	{
//		m_property->m_isOn = on;
//	}
//}

//void CLight::SetRange( const float range )
//{
//	if ( m_property )
//	{
//		m_property->m_range = range;
//	}
//}

//void CLight::SetFallOff( const float fallOff )
//{
//	if ( m_property )
//	{
//		m_property->m_fallOff = fallOff;
//	}
//}

//void CLight::SetConeProperty( const float theta, const float phi )
//{
//	if ( m_property )
//	{
//		m_property->m_theta = theta;
//		m_property->m_phi = phi;
//	}
//}

//void CLight::SetDiection( const CXMFLOAT3& direction )
//{
//	if ( m_property )
//	{
//		m_property->m_direction = XMVector3Normalize( direction );
//	}
//}

//void CLight::SetAttenuation( const CXMFLOAT3& attenuation )
//{
//	if ( m_property )
//	{
//		m_property->m_attenuation = attenuation;
//	}
//}

void Light::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	if ( const JSON::Value* pDiffuse = json.Find( "Diffuse" ) )
	{
		const JSON::Value& diffuse = *pDiffuse;

		if ( diffuse.Size( ) >= 4 )
		{
			float r = static_cast<float>( diffuse[0].AsReal( ) );
			float g = static_cast<float>( diffuse[1].AsReal( ) );
			float b = static_cast<float>( diffuse[2].AsReal( ) );
			float a = static_cast<float>( diffuse[3].AsReal( ) );

			SetDiffuseColor( CXMFLOAT4( r, g, b, a ) );
		}
	}

	if ( const JSON::Value* pSpecular = json.Find( "Specular" ) )
	{
		const JSON::Value& specluar = *pSpecular;

		if ( specluar.Size( ) >= 4 )
		{
			float r = static_cast<float>( specluar[0].AsReal( ) );
			float g = static_cast<float>( specluar[1].AsReal( ) );
			float b = static_cast<float>( specluar[2].AsReal( ) );
			float a = static_cast<float>( specluar[3].AsReal( ) );

			SetSpecularColor( CXMFLOAT4( r, g, b, a ) );
		}
	}
}

void Light::SetDiffuseColor( const CXMFLOAT4& diffuseColor )
{
	GetLightComponent( ).SetDiffuseColor( diffuseColor );
}

void Light::SetSpecularColor( const CXMFLOAT4& specularColor )
{
	GetLightComponent( ).SetSpecularColor( specularColor );
}

LightComponent& Light::GetLightComponent( )
{
	if ( m_component == nullptr )
	{
		m_component = GetComponent<LightComponent>( );
	}

	return *m_component;
}

DECLARE_GAME_OBJECT( directional_light, DirectionalLight );

const LIGHT_TYPE DirectionalLight::GetType( ) const
{
	return LIGHT_TYPE::DIRECTINAL_LIGHT;
}

void DirectionalLight::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	Light::LoadProperty( gameLogic, json );

	if ( const JSON::Value* pDirection = json.Find( "Direction" ) )
	{
		const JSON::Value& direction = *pDirection;

		if ( direction.Size( ) >= 3 )
		{
			float x = static_cast<float>( direction[0].AsReal( ) );
			float y = static_cast<float>( direction[1].AsReal( ) );
			float z = static_cast<float>( direction[2].AsReal( ) );

			CXMFLOAT3 vDir( x, y, z );
			vDir = XMVector3Normalize( vDir );

			m_directionalLightComponent->SetDirection( vDir );
		}
	}
}

const CXMFLOAT3& DirectionalLight::Direction( ) const
{
	return m_directionalLightComponent->Direction( );
}

DirectionalLight::DirectionalLight( )
{
	m_directionalLightComponent = CreateComponent<DirectionalLightComponent>( *this );
	m_rootComponent = m_directionalLightComponent;
}

DECLARE_GAME_OBJECT( hemisphere_light, HemisphereLight );

void HemisphereLight::LoadProperty( CGameLogic& gameLogic, const JSON::Value& json )
{
	if ( const JSON::Value* pLowerColor = json.Find( "LowerColor" ) )
	{
		const JSON::Value& lowerColor = *pLowerColor;

		if ( lowerColor.Size( ) >= 4 )
		{
			float r = static_cast<float>( lowerColor[0].AsReal( ) );
			float g = static_cast<float>( lowerColor[1].AsReal( ) );
			float b = static_cast<float>( lowerColor[2].AsReal( ) );
			float a = static_cast<float>( lowerColor[3].AsReal( ) );

			SetLowerColor( CXMFLOAT4( r, g, b, a ) );
		}
	}

	if ( const JSON::Value* pUpperColor = json.Find( "UpperColor" ) )
	{
		const JSON::Value& upperColor = *pUpperColor;

		if ( upperColor.Size( ) >= 4 )
		{
			float r = static_cast<float>( upperColor[0].AsReal( ) );
			float g = static_cast<float>( upperColor[1].AsReal( ) );
			float b = static_cast<float>( upperColor[2].AsReal( ) );
			float a = static_cast<float>( upperColor[3].AsReal( ) );

			SetUpperColor( CXMFLOAT4( r, g, b, a ) );
		}
	}
}

void HemisphereLight::SetLowerColor( const CXMFLOAT4& color )
{
	if ( m_hemisphereLightComponent )
	{
		m_hemisphereLightComponent->SetLowerColor( color );
	}
}

void HemisphereLight::SetUpperColor( const CXMFLOAT4& color )
{
	if ( m_hemisphereLightComponent )
	{
		m_hemisphereLightComponent->SetUpperColor( color );
	}
}

HemisphereLight::HemisphereLight( )
{
	m_hemisphereLightComponent = CreateComponent<HemisphereLightComponent>( *this );
	m_rootComponent = m_hemisphereLightComponent;
}
