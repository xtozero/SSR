#include "stdafx.h"
#include "Proxies/StaticMeshPrimitiveProxy.h"

#include "Components/StaticMeshComponent.h"

StaticMeshPrimitiveProxy::StaticMeshPrimitiveProxy( const StaticMeshComponent& component ) : m_pStaticMesh( component.GetStaticMesh( ) )
{
}
