#include "stdafx.h"
#include "Components/StaticMeshComponent.h"

#include "Mesh/StaticMesh.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include <cassert>

BoxSphereBounds StaticMeshComponent::CalcBounds( const Matrix& transform )
{
	if ( m_pStaticMesh )
	{
		BoxSphereBounds bounds = m_pStaticMesh->Bounds( ).TransformBy( transform );
		return bounds;
	}

	Vector position( transform._41, transform._42, transform._43 );
	return BoxSphereBounds( position, Vector( 0, 0, 0 ), 0.f );
}

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

	UpdateBounds( );
}

void StaticMeshComponent::SetRenderOption( const std::shared_ptr<RenderOption>& pRenderOption )
{
	assert( pRenderOption != nullptr );
	m_pRenderOption = pRenderOption;

	MarkRenderStateDirty( );
}
