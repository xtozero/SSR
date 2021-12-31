#include "stdafx.h"
#include "Components/LightComponent.h"

#include "Math/TransformationMatrix.h"
#include "Proxies/LightProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

void LightComponent::SetDiffuseColor( const ColorF& diffuseColor )
{
	m_diffuse = diffuseColor;
}

void LightComponent::SetSpecularColor( const ColorF& specularColor )
{
	m_specular = specularColor;
}

LightProxy* DirectionalLightComponent::CreateProxy( ) const
{
	return new DirectionalLightProxy( *this );
}

void DirectionalLightComponent::SetDirection( const Vector& direction )
{
	m_direction = direction;
}

bool DirectionalLightComponent::ShouldCreateRenderState( ) const
{
	return true;
}

void DirectionalLightComponent::CreateRenderState( )
{
	LightComponent::CreateRenderState( );
	m_pWorld->Scene( )->AddLight( this );
}

void DirectionalLightComponent::RemoveRenderState( )
{
	LightComponent::RemoveRenderState( );
	m_pWorld->Scene( )->RemoveLight( this );
}

HemisphereLightProxy* HemisphereLightComponent::CreateProxy( ) const
{
	return new HemisphereLightProxy( LowerColor( ), UpperColor( ), UpVector( ) );
}

Vector HemisphereLightComponent::UpVector( ) const
{
	using namespace DirectX;

	RotateMatrix rotate( GetRotate( ) );
	return rotate.TransformVector( Vector::UpVector );
}

bool HemisphereLightComponent::ShouldCreateRenderState( ) const
{
	return true;
}

void HemisphereLightComponent::CreateRenderState( )
{
	SceneComponent::CreateRenderState( );
	m_pWorld->Scene( )->AddHemisphereLightComponent( this );
}

void HemisphereLightComponent::RemoveRenderState( )
{
	SceneComponent::RemoveRenderState( );
	m_pWorld->Scene( )->RemoveHemisphereLightComponent( this );
}
