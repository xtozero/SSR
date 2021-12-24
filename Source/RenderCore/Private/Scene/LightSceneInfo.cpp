#include "stdafx.h"
#include "Scene/LightSceneInfo.h"

#include "Components/LightComponent.h"
#include "Proxies/LightProxy.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"

PrimitiveIntersectionInfo::~PrimitiveIntersectionInfo( )
{
	if ( m_primitive )
	{
		m_primitive->Lights( ).RemoveAt( m_infoId );
	}
}

SparseArray<PrimitiveIntersectionInfo>& LightSceneInfo::Primitives( )
{
	return m_primitiveList;
}

const SparseArray<PrimitiveIntersectionInfo>& LightSceneInfo::Primitives( ) const
{
	return m_primitiveList;
}

void LightSceneInfo::AddToScene( )
{
	for ( PrimitiveSceneInfo* primitive : m_scene.Primitives( ) )
	{
		AddPrimitiveIntersectionInfo( *primitive );
	}
}

void LightSceneInfo::RemoveFromScene( )
{
	m_primitiveList.Clear( );
}

void LightSceneInfo::AddPrimitiveIntersectionInfo( PrimitiveSceneInfo& primitive )
{
	if ( AffactsPrimitive( primitive ) )
	{
		auto primitiveInfoId = static_cast<uint32>( m_primitiveList.AddUninitialized( ) );

		SparseArray<LightIntersectionInfo>& lights = primitive.Lights( );
		auto lightInfoId = static_cast<uint32>( lights.AddUninitialized( ) );

		new (&m_primitiveList[primitiveInfoId]) PrimitiveIntersectionInfo( &primitive, lightInfoId );
		new (&lights[lightInfoId]) LightIntersectionInfo( this, primitiveInfoId );
	}
}

LightSceneInfo::LightSceneInfo( LightComponent& component, Scene& scene ) : m_lightProxy( component.m_lightProxy ), m_scene( scene ), m_shadowQuility( static_cast<uint32>( component.GetShadowQuility( ) ) )
{
}

bool LightSceneInfo::AffactsPrimitive( PrimitiveSceneInfo& primitive )
{
	uint32 primitiveId = primitive.PrimitiveId( );
	const BoxSphereBounds& primitiveBound = m_scene.PrimitiveBounds( )[primitiveId];

	if ( m_lightProxy->AffactsBounds( primitiveBound ) == false )
	{
		return false;
	}

	return true;
}
