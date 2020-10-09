#include "stdafx.h"
#include "Components/StaticMeshComponent.h"

#include "Model/StaticMesh.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include <cassert>

PrimitiveProxy* StaticMeshComponent::CreateProxy( ) const
{
	if ( m_pStaticMesh == nullptr || m_pStaticMesh->RenderData( ) == nullptr )
	{
		return nullptr;
	}

	return new StaticMeshPrimitiveProxy( *this );
}

void StaticMeshComponent::SetStaticMesh( StaticMesh* pStaticMesh )
{
	assert( pStaticMesh != nullptr );
	m_pStaticMesh = pStaticMesh;

	MarkRenderStateDirty( );
}
