#include "stdafx.h"
#include "Components/LightComponent.h"

#include "Proxies/LightProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

void LightComponent::SetDiffuseColor( const CXMFLOAT4& diffuseColor )
{
	m_diffuse = diffuseColor;
}

void LightComponent::SetSpecularColor( const CXMFLOAT4& specularColor )
{
	m_specular = specularColor;
}

LightProxy* DirectionalLightComponent::CreateProxy( ) const
{
	return new DirectionalLightProxy( *this );
}

void DirectionalLightComponent::SetDirection( const CXMFLOAT3& direction )
{
	m_direction = direction;
}

bool DirectionalLightComponent::ShouldCreateRenderState( ) const
{
	return true;
}

void DirectionalLightComponent::CreateRenderState( )
{
	SceneComponent::CreateRenderState( );
	m_pWorld->Scene( )->AddLight( this );
}

void DirectionalLightComponent::RemoveRenderState( )
{
	SceneComponent::RemoveRenderState( );
	m_pWorld->Scene( )->RemoveLight( this );
}
