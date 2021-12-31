#include "stdafx.h"
#include "GameObject/Light.h"

#include "Components/LightComponent.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

#include <cassert>

using namespace DirectX;

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

			SetDiffuseColor( ColorF( r, g, b, a ) );
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

			SetSpecularColor( ColorF( r, g, b, a ) );
		}
	}

	if ( const JSON::Value* pCastShadow = json.Find( "CastShadow" ) )
	{
		const JSON::Value& castShadow = *pCastShadow;

		SetCastShadow( castShadow.AsBool( ) );
	}
}

void Light::SetDiffuseColor( const ColorF& diffuseColor )
{
	GetLightComponent( ).SetDiffuseColor( diffuseColor );
}

void Light::SetSpecularColor( const ColorF& specularColor )
{
	GetLightComponent( ).SetSpecularColor( specularColor );
}

void Light::SetCastShadow( bool castShadow )
{
	GetLightComponent( ).CastShadow( ) = castShadow;
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

			Vector vDir( x, y, z );
			m_directionalLightComponent->SetDirection( vDir.GetNormalized() );
		}
	}
}

const Vector& DirectionalLight::Direction( ) const
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

			SetLowerColor( ColorF( r, g, b, a ) );
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

			SetUpperColor( ColorF( r, g, b, a ) );
		}
	}
}

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

HemisphereLight::HemisphereLight( )
{
	m_hemisphereLightComponent = CreateComponent<HemisphereLightComponent>( *this );
	m_rootComponent = m_hemisphereLightComponent;
}
