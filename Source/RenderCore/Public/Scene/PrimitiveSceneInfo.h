#pragma once

#include "DrawSnapshot.h"
#include "SizedTypes.h"

#include <cstddef>
#include <vector>

class PrimitiveComponent;
class PrimitiveProxy;
class Scene;

struct PrimitiveSubMeshInfo
{
	uint32 m_snapshotInfoBase = 0;
};

class PrimitiveSubMesh
{
public:
	uint32& Lod( )
	{
		return m_lod;
	}

	const uint32 Lod( ) const
	{
		return m_lod;
	}

	uint32& SectionIndex( )
	{
		return m_sectionIndex;
	}

	const uint32 SectionIndex( ) const
	{
		return m_sectionIndex;
	}

private:
	uint32 m_lod = 0;
	uint32 m_sectionIndex = 0;
};

class PrimitiveSceneInfo
{
public:
	PrimitiveProxy*& Proxy( );
	const PrimitiveProxy* Proxy( ) const;

	uint32& PrimitiveId( );
	uint32 PrimitiveId( ) const;

	void AddToScene( );
	void RemoveFromScene( );

	PrimitiveSubMesh& AddSubMesh( );

	std::vector<PrimitiveSubMeshInfo>& SubMeshInfos( );
	const std::vector<PrimitiveSubMeshInfo>& SubMeshInfos( ) const;

	std::vector<PrimitiveSubMesh>& SubMeshs( );
	const std::vector<PrimitiveSubMesh>& SubMeshs( ) const;

	const CachedDrawSnapshotInfo& GetCachedDrawSnapshotInfo( uint32 snapshotIndex );

	DrawSnapshot& CachedDrawSnapshot( uint32 snapshotIndex );

	PrimitiveSceneInfo( PrimitiveComponent* component, Scene& scene );

private:
	void CacheDrawSnapshot( );
	void RemoveCachedDrawSnapshot( );

	Scene& m_scene;

	PrimitiveProxy* m_sceneProxy = nullptr;

	uint32 m_primitiveId = 0;

	std::vector<PrimitiveSubMeshInfo> m_subMeshInfos;
	std::vector<PrimitiveSubMesh> m_subMeshs;
	std::vector<CachedDrawSnapshotInfo> m_cachedDrawSnapshotInfos;
};