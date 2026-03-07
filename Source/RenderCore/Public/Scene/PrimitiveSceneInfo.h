#pragma once

#include "HitProxy.h"
#include "MeshDrawInfo.h"
#include "PassProcessor.h"
#include "Scene.h"
#include "SizedTypes.h"
#include "SparseArray.h"

#include <optional>
#include <vector>

namespace logic
{
	class PrimitiveComponent;
}

namespace rendercore
{
	class DrawSnapshot;
	class LightSceneInfo;
	class PrimitiveProxy;
	class RaytracingScene;
	class ShadingSnapshot;

	class PrimitiveSubMeshInfo final
	{
	public:
		std::optional<uint32> GetCachedDrawSnapshotInfoIndex( RenderPassType passType ) const;
		int32 GetShadingSnapshotId() const;

		void OnDrawSnapshotAdded( RenderPassType passType );

		void SetDrawSnapshotInfoBase( uint32 snapshotInfoBase );
		void SetShadingSnapshotId( int32 shadingSnapshotId );

	private:
		uint32 m_drawSnapshotInfoBase = 0;
		uint32 m_passTypeMask = 0;

		int32 m_shadingSnapshotId = -1;
	};

	struct PrimitiveSubMesh : public MeshDrawInfo
	{
		explicit PrimitiveSubMesh( const MeshDrawInfo& info ) : MeshDrawInfo( info ) {}
		PrimitiveSubMesh() = default;
	};

	struct FullScreenQuadDrawInfo : public PrimitiveSubMesh
	{
		FullScreenQuadDrawInfo();
	};

	struct LightIntersectionInfo final
	{
		LightSceneInfo* m_light = nullptr;
		uint32 m_infoId = 0;

		LightIntersectionInfo( LightSceneInfo* light, uint32 infoId ) : m_light( light ), m_infoId( infoId ) {}
	};

	class PrimitiveSceneInfo final
	{
	public:
		PrimitiveProxy*& Proxy();
		const PrimitiveProxy* Proxy() const;

		uint32& PrimitiveId();
		uint32 PrimitiveId() const;

		void AddToScene();
		void AddToRaytracingScene( RaytracingScene& raytracingScene );
		void RemoveFromScene();
		void RemoveFromRaytracingScene( RaytracingScene& raytracingScene );

		PrimitiveSubMesh& AddSubMesh();

		std::vector<PrimitiveSubMeshInfo>& SubMeshInfos();
		const std::vector<PrimitiveSubMeshInfo>& SubMeshInfos() const;

		std::vector<PrimitiveSubMesh>& SubMeshs();
		const std::vector<PrimitiveSubMesh>& SubMeshs() const;

		SparseArray<LightIntersectionInfo>& Lights();
		const SparseArray<LightIntersectionInfo>& Lights() const;

		const CachedDrawSnapshotInfo& GetCachedDrawSnapshotInfo( uint32 snapshotInfoBase );

		DrawSnapshot& GetCachedDrawSnapshot( uint32 snapshotIndex );
		ShadingSnapshot& GetShadingSnapshot( int32 shadingSnapshotId );

		HitProxyId GetHitProxyId() const;

		PrimitiveSceneInfo( logic::PrimitiveComponent* component, Scene& scene );

	private:
		friend void Scene::PreReloadShaders();
		friend void Scene::PostReloadShaders();
		friend RaytracingScene;

		void CacheDrawSnapshot();
		void RemoveCachedDrawSnapshot();

		void CacheShadingSnapshot();
		void RemoveCachedShadingSnapshot();

		Scene& m_scene;

		PrimitiveProxy* m_sceneProxy = nullptr;
		RefHandle<HitProxy> m_hitProxy;

		uint32 m_primitiveId = 0;
		uint32 m_rayTracingInstanceId = RaytracingScene::InvalidInstanceId;

		std::vector<PrimitiveSubMeshInfo> m_subMeshInfos;
		std::vector<PrimitiveSubMesh> m_subMeshs;
		std::vector<CachedDrawSnapshotInfo> m_cachedDrawSnapshotInfos;

		std::vector<int32> m_cachedShadingSnapshotIds;

		SparseArray<LightIntersectionInfo> m_lightList;
	};
}
