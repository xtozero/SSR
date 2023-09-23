#pragma once

#include "SizedTypes.h"
#include "SparseArray.h"

#include <cstddef>

namespace logic
{
	class LightComponent;
}

namespace rendercore
{
	class LightProxy;
	class PrimitiveSceneInfo;
	class Scene;

	struct PrimitiveIntersectionInfo
	{
		PrimitiveSceneInfo* m_primitive = nullptr;
		uint32 m_infoId = 0;

		PrimitiveIntersectionInfo( PrimitiveSceneInfo* primitive, uint32 infoId ) : m_primitive( primitive ), m_infoId( infoId ) {}
	};

	class LightSceneInfo
	{
	public:
		const LightProxy* Proxy() const
		{
			return m_lightProxy;
		}

		LightProxy*& Proxy()
		{
			return m_lightProxy;
		}

		uint32 ID() const
		{
			return m_id;
		}

		void SetID( uint32 id )
		{
			m_id = id;
		}

		uint32 GetShadowQuility() const
		{
			return m_shadowQuility;
		}

		SparseArray<PrimitiveIntersectionInfo>& Primitives();
		const SparseArray<PrimitiveIntersectionInfo>& Primitives() const;

		void AddToScene();
		void RemoveFromScene();
		void AddPrimitiveIntersectionInfo( PrimitiveSceneInfo& primitive );
		void RemovePrimitiveIntersectionInfo( PrimitiveSceneInfo& primitive );

		LightSceneInfo( logic::LightComponent& component, Scene& scene );

	private:
		bool AffactsPrimitive( PrimitiveSceneInfo& primitive );

		uint32 m_id = 0;
		LightProxy* m_lightProxy = nullptr;
		Scene& m_scene;

		uint32 m_shadowQuility = 0;

		SparseArray<PrimitiveIntersectionInfo> m_primitiveList;
	};
}
