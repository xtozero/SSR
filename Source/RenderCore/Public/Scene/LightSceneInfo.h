#pragma once

#include "PrimitiveProxy.h"
#include "PrimitiveSceneInfo.h"
#include "SizedTypes.h"
#include "SparseArray.h"

namespace logic
{
	class LightComponent;
}

namespace rendercore
{
	class LightProxy;
	class PrimitiveSceneInfo;
	class Scene;

	struct PrimitiveIntersectionInfo final
	{
		PrimitiveSceneInfo* m_primitive = nullptr;
		uint32 m_infoId = 0;
		bool m_castShadow = false;

		PrimitiveIntersectionInfo( PrimitiveSceneInfo* primitive, uint32 infoId ) 
			: m_primitive( primitive )
			, m_infoId( infoId ) 
		{
			m_castShadow = m_primitive->Proxy()->CastShadow();
		}
	};

	class LightSceneInfo final
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

		uint32 Id() const
		{
			return m_id;
		}

		void SetId( uint32 id )
		{
			m_id = id;
		}

		// Valid in current frame only. Do not cache this id
		uint32 IdOnGPU() const
		{
			return m_idOnGpu;
		}

		void SetIdOnGPU( uint32 id )
		{
			m_idOnGpu = id;
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
		uint32 m_idOnGpu = 0;
		LightProxy* m_lightProxy = nullptr;
		Scene& m_scene;

		uint32 m_shadowQuility = 0;

		SparseArray<PrimitiveIntersectionInfo> m_primitiveList;
	};
}
