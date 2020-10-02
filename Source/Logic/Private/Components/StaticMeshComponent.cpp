#include "stdafx.h"
#include "Components/StaticMeshComponent.h"

#include <cassert>

void StaticMeshComponent::SetStaticMesh( StaticMesh* pStaticMesh )
{
	assert( pStaticMesh != nullptr );
	m_pStaticMesh = pStaticMesh;
}
