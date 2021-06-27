#include "stdafx.h"
#include "Components/TexturedSkyComponent.h"

#include "Proxies/TexturedSkyProxy.h"
#include "Scene/IScene.h"
#include "World/World.h"

void TexturedSkyComponent::CreateRenderState( )
{
	Component::CreateRenderState( );
	m_pWorld->Scene( )->AddTexturedSkyComponent( this );
}

void TexturedSkyComponent::RemoveRenderState( )
{
	Component::RemoveRenderState( );
	m_pWorld->Scene( )->RemoveTexturedSkyComponent( this );
}

TexturedSkyProxy* TexturedSkyComponent::CreateProxy( ) const
{
	if ( m_pStaticMesh == nullptr ||
		m_pMaterial == nullptr )
	{
		return nullptr;
	}

	return new TexturedSkyProxy( *this );
}

void TexturedSkyComponent::SetStaticMesh( const std::shared_ptr<StaticMesh>& pStaticMesh )
{
	assert( pStaticMesh != nullptr );
	m_pStaticMesh = pStaticMesh;

	MarkRenderStateDirty( );
}

void TexturedSkyComponent::SetMaterial( const std::shared_ptr<Material>& pMaterial )
{
	assert( pMaterial != nullptr );
	m_pMaterial = pMaterial;

	MarkRenderStateDirty( );
}

bool TexturedSkyComponent::ShouldCreateRenderState( ) const
{
	return true;
}
