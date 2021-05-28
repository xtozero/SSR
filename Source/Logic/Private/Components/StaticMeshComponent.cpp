#include "stdafx.h"
#include "Components/StaticMeshComponent.h"

#include "Mesh/StaticMesh.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include <cassert>

PrimitiveProxy* StaticMeshComponent::CreateProxy( ) const
{
	if ( m_pStaticMesh == nullptr 
		|| m_pStaticMesh->RenderData( ) == nullptr
		|| m_pRenderOption == nullptr )
	{
		return nullptr;
	}

	return new StaticMeshPrimitiveProxy( *this );
}

void StaticMeshComponent::SetStaticMesh( const std::shared_ptr<StaticMesh>& pStaticMesh )
{
	assert( pStaticMesh != nullptr );
	m_pStaticMesh = pStaticMesh;

	MarkRenderStateDirty( );
}

void StaticMeshComponent::SetRenderOption( const std::shared_ptr<RenderOption>& pRenderOption )
{
	assert( pRenderOption != nullptr );
	m_pRenderOption = pRenderOption;

	MarkRenderStateDirty( );
}
