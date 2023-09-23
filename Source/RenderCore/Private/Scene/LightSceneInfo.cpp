#include "stdafx.h"
#include "Scene/LightSceneInfo.h"

#include "Components/LightComponent.h"
#include "Proxies/LightProxy.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "Scene/Scene.h"

namespace rendercore
{
	SparseArray<PrimitiveIntersectionInfo>& LightSceneInfo::Primitives()
	{
		return m_primitiveList;
	}

	const SparseArray<PrimitiveIntersectionInfo>& LightSceneInfo::Primitives() const
	{
		return m_primitiveList;
	}

	void LightSceneInfo::AddToScene()
	{
		for ( PrimitiveSceneInfo* primitive : m_scene.Primitives() )
		{
			AddPrimitiveIntersectionInfo( *primitive );
		}
	}

	void LightSceneInfo::RemoveFromScene()
	{
		for ( PrimitiveSceneInfo* primitive : m_scene.Primitives() )
		{
			RemovePrimitiveIntersectionInfo( *primitive );
		}

		assert( m_primitiveList.Size() == 0 );
	}

	void LightSceneInfo::AddPrimitiveIntersectionInfo( PrimitiveSceneInfo& primitive )
	{
		if ( AffactsPrimitive( primitive ) )
		{
			auto primitiveInfoId = static_cast<uint32>( m_primitiveList.AddUninitialized() );

			SparseArray<LightIntersectionInfo>& lights = primitive.Lights();
			auto lightInfoId = static_cast<uint32>( lights.AddUninitialized() );

			std::construct_at( &m_primitiveList[primitiveInfoId], &primitive, lightInfoId );
			std::construct_at( &lights[lightInfoId], this, primitiveInfoId );
		}
	}

	void LightSceneInfo::RemovePrimitiveIntersectionInfo( PrimitiveSceneInfo& primitive )
	{
		SparseArray<LightIntersectionInfo>& lights = primitive.Lights();

		for ( size_t i = 0; i < lights.Size(); ++i )
		{
			if ( lights[i].m_light == this )
			{
				m_primitiveList.RemoveAt( lights[i].m_infoId );
				lights.RemoveAt( i );
			}
		}
	}

	LightSceneInfo::LightSceneInfo( logic::LightComponent& component, Scene& scene ) : m_lightProxy( component.m_lightProxy ), m_scene( scene ), m_shadowQuility( static_cast<uint32>( component.GetShadowQuility() ) )
	{
	}

	bool LightSceneInfo::AffactsPrimitive( PrimitiveSceneInfo& primitive )
	{
		uint32 primitiveId = primitive.PrimitiveId();
		const BoxSphereBounds& primitiveBound = m_scene.PrimitiveBounds()[primitiveId];

		if ( m_lightProxy->AffactsBounds( primitiveBound ) == false )
		{
			return false;
		}

		return true;
	}
}
