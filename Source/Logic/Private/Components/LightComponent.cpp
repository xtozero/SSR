#include "stdafx.h"
#include "Components/LightComponent.h"

#include "Json/Json.hpp"
#include "Math/TransformationMatrix.h"
#include "Proxies/LightProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

void LightComponent::LoadProperty( const JSON::Value& json )
{
	Super::LoadProperty( json );

	if ( const JSON::Value* pDiffuse = json.Find( "Diffuse" ) )
	{
		const JSON::Value& diffuse = *pDiffuse;

		if ( diffuse.Size() >= 4 )
		{
			float r = static_cast<float>( diffuse[0].AsReal() );
			float g = static_cast<float>( diffuse[1].AsReal() );
			float b = static_cast<float>( diffuse[2].AsReal() );
			float a = static_cast<float>( diffuse[3].AsReal() );

			SetDiffuseColor( ColorF( r, g, b, a ) );
		}
	}

	if ( const JSON::Value* pSpecular = json.Find( "Specular" ) )
	{
		const JSON::Value& specluar = *pSpecular;

		if ( specluar.Size() >= 4 )
		{
			float r = static_cast<float>( specluar[0].AsReal() );
			float g = static_cast<float>( specluar[1].AsReal() );
			float b = static_cast<float>( specluar[2].AsReal() );
			float a = static_cast<float>( specluar[3].AsReal() );

			SetSpecularColor( ColorF( r, g, b, a ) );
		}
	}

	if ( const JSON::Value* pCastShadow = json.Find( "CastShadow" ) )
	{
		const JSON::Value& castShadow = *pCastShadow;

		CastShadow() = castShadow.AsBool();
	}
}

void LightComponent::SetDiffuseColor( const ColorF& diffuseColor )
{
	m_diffuse = diffuseColor;
}

void LightComponent::SetSpecularColor( const ColorF& specularColor )
{
	m_specular = specularColor;
}

void DirectionalLightComponent::LoadProperty( const JSON::Value& json )
{
	Super::LoadProperty( json );

	if ( const JSON::Value* pDirection = json.Find( "Direction" ) )
	{
		const JSON::Value& direction = *pDirection;

		if ( direction.Size() >= 3 )
		{
			float x = static_cast<float>( direction[0].AsReal() );
			float y = static_cast<float>( direction[1].AsReal() );
			float z = static_cast<float>( direction[2].AsReal() );

			Vector vDir( x, y, z );
			SetDirection( vDir.GetNormalized() );
		}
	}

	if ( const JSON::Value* pUsedAsAtmosphereSunLight = json.Find( "UsedAsAtmosphereSunLight" ) )
	{
		bool usedAsAtmosphereSunLight = pUsedAsAtmosphereSunLight->AsBool();
		SetUsedAsAtmosphereSunLight( usedAsAtmosphereSunLight );
	}
}

rendercore::LightProxy* DirectionalLightComponent::CreateProxy() const
{
	return new rendercore::DirectionalLightProxy( *this );
}

void DirectionalLightComponent::SetDirection( const Vector& direction )
{
	m_direction = direction;
}

bool DirectionalLightComponent::ShouldCreateRenderState() const
{
	return true;
}

void DirectionalLightComponent::CreateRenderState()
{
	Super::CreateRenderState();
	m_pWorld->Scene()->AddLight( this );
}

void DirectionalLightComponent::RemoveRenderState()
{
	Super::RemoveRenderState();
	m_pWorld->Scene()->RemoveLight( this );
}

void HemisphereLightComponent::LoadProperty( const JSON::Value& json )
{
	Super::LoadProperty( json );

	if ( const JSON::Value* pLowerColor = json.Find( "LowerColor" ) )
	{
		const JSON::Value& lowerColor = *pLowerColor;

		if ( lowerColor.Size() >= 4 )
		{
			float r = static_cast<float>( lowerColor[0].AsReal() );
			float g = static_cast<float>( lowerColor[1].AsReal() );
			float b = static_cast<float>( lowerColor[2].AsReal() );
			float a = static_cast<float>( lowerColor[3].AsReal() );

			SetLowerColor( ColorF( r, g, b, a ) );
		}
	}

	if ( const JSON::Value* pUpperColor = json.Find( "UpperColor" ) )
	{
		const JSON::Value& upperColor = *pUpperColor;

		if ( upperColor.Size() >= 4 )
		{
			float r = static_cast<float>( upperColor[0].AsReal() );
			float g = static_cast<float>( upperColor[1].AsReal() );
			float b = static_cast<float>( upperColor[2].AsReal() );
			float a = static_cast<float>( upperColor[3].AsReal() );

			SetUpperColor( ColorF( r, g, b, a ) );
		}
	}
}

rendercore::HemisphereLightProxy* HemisphereLightComponent::CreateProxy() const
{
	return new rendercore::HemisphereLightProxy( LowerColor(), UpperColor(), UpVector() );
}

Vector HemisphereLightComponent::UpVector() const
{
	RotationMatrix rotation( GetRotation() );
	return rotation.TransformVector( Vector::UpVector );
}

bool HemisphereLightComponent::ShouldCreateRenderState() const
{
	return true;
}

void HemisphereLightComponent::CreateRenderState()
{
	Super::CreateRenderState();
	m_pWorld->Scene()->AddHemisphereLightComponent( this );
}

void HemisphereLightComponent::RemoveRenderState()
{
	Super::RemoveRenderState();
	m_pWorld->Scene()->RemoveHemisphereLightComponent( this );
}
