#pragma once

#include "DrawSnapshot.h"
#include "MeshDrawInfo.h"
#include "PassProcessor.h"
#include "SizedTypes.h"
#include "SparseArray.h"

#include <cstddef>
#include <optional>
#include <vector>

class LightSceneInfo;
class PrimitiveComponent;
class PrimitiveProxy;
class Scene;

class PrimitiveSubMeshInfo
{
public:
	std::optional<uint32> GetCachedDrawSnapshotInfoIndex( RenderPass passType ) const;

	void OnDrawSnapshotAdded( RenderPass passType );

	uint32& SnapshotInfoBase();
	uint32 SnapshotInfoBase() const;

private:
	uint32 m_snapshotInfoBase = 0;
	uint32 m_passTypeMask = 0;
};

struct PrimitiveSubMesh : public MeshDrawInfo
{
	explicit PrimitiveSubMesh( const MeshDrawInfo& info ) : MeshDrawInfo( info ) {}
	PrimitiveSubMesh() = default;
};

struct LightIntersectionInfo
{
	LightSceneInfo* m_light = nullptr;
	uint32 m_infoId = 0;

	LightIntersectionInfo( LightSceneInfo* light, uint32 infoId ) : m_light( light ), m_infoId( infoId ) {}
};

class PrimitiveSceneInfo
{
public:
	PrimitiveProxy*& Proxy();
	const PrimitiveProxy* Proxy() const;

	uint32& PrimitiveId();
	uint32 PrimitiveId() const;

	void AddToScene();
	void RemoveFromScene();

	PrimitiveSubMesh& AddSubMesh();

	std::vector<PrimitiveSubMeshInfo>& SubMeshInfos();
	const std::vector<PrimitiveSubMeshInfo>& SubMeshInfos() const;

	std::vector<PrimitiveSubMesh>& SubMeshs();
	const std::vector<PrimitiveSubMesh>& SubMeshs() const;

	SparseArray<LightIntersectionInfo>& Lights();
	const SparseArray<LightIntersectionInfo>& Lights() const;

	const CachedDrawSnapshotInfo& GetCachedDrawSnapshotInfo( uint32 snapshotInfoBase );

	DrawSnapshot& CachedDrawSnapshot( uint32 snapshotIndex );

	PrimitiveSceneInfo( PrimitiveComponent* component, Scene& scene );

private:
	void CacheDrawSnapshot();
	void RemoveCachedDrawSnapshot();

	Scene& m_scene;

	PrimitiveProxy* m_sceneProxy = nullptr;

	uint32 m_primitiveId = 0;

	std::vector<PrimitiveSubMeshInfo> m_subMeshInfos;
	std::vector<PrimitiveSubMesh> m_subMeshs;
	std::vector<CachedDrawSnapshotInfo> m_cachedDrawSnapshotInfos;

	SparseArray<LightIntersectionInfo> m_lightList;
};