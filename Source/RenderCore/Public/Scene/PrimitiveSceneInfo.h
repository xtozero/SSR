#pragma once
#include "DrawSnapshot.h"

#include <cstddef>
#include <vector>

class PrimitiveComponent;
class PrimitiveProxy;
class Scene;

struct PrimitiveSubMeshInfo
{
	std::size_t m_snapshotInfoBase = 0;
};

class PrimitiveSubMesh
{
public:
	std::size_t& Lod( )
	{
		return m_lod;
	}

	const std::size_t Lod( ) const
	{
		return m_lod;
	}

	std::size_t& SectionIndex( )
	{
		return m_sectionIndex;
	}

	const std::size_t SectionIndex( ) const
	{
		return m_sectionIndex;
	}

private:
	std::size_t m_lod = 0;
	std::size_t m_sectionIndex = 0;
};

class PrimitiveSceneInfo
{
public:
	void AddToScene( );
	void RemoveFromScene( );

	PrimitiveSubMesh& AddSubMesh( );

	std::vector<PrimitiveSubMeshInfo>& SubMeshInfos( );
	const std::vector<PrimitiveSubMeshInfo>& SubMeshInfos( ) const;

	std::vector<PrimitiveSubMesh>& SubMeshs( );
	const std::vector<PrimitiveSubMesh>& SubMeshs( ) const;

	DrawSnapshot& CachedDrawSnapshot( std::size_t snapshotIndex );

	PrimitiveSceneInfo( PrimitiveComponent* component, Scene& scene );

	PrimitiveProxy* m_sceneProxy = nullptr;
	std::size_t m_primitiveId = 0;

private:
	void CacheDrawSnapshot( );
	void RemoveCachedDrawSnapshot( );

	Scene& m_scene;

	std::vector<PrimitiveSubMeshInfo> m_subMeshInfos;
	std::vector<PrimitiveSubMesh> m_subMeshs;
	std::vector<CachedDrawSnapshotInfo> m_cachedDrawSnapshotInfos;
};