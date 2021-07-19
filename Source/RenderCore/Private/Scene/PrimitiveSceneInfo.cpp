#include "stdafx.h"
#include "Scene/PrimitiveSceneInfo.h"

#include "Components/PrimitiveComponent.h"
#include "Proxies/PrimitiveProxy.h"
#include "Scene/Scene.h"

void PrimitiveSceneInfo::AddToScene( )
{
	m_sceneProxy->PrepareSubMeshs( );

	CacheDrawSnapshot( );
}

void PrimitiveSceneInfo::RemoveFromScene( )
{
	m_subMeshInfos.clear( );
	m_subMeshs.clear( );
	RemoveCachedDrawSnapshot( );
}

PrimitiveSubMesh& PrimitiveSceneInfo::AddSubMesh( )
{
	m_subMeshInfos.emplace_back( );
	PrimitiveSubMesh& subMesh = m_subMeshs.emplace_back( );
	return subMesh;
}

std::vector<PrimitiveSubMeshInfo>& PrimitiveSceneInfo::SubMeshInfos( )
{
	return m_subMeshInfos;
}

const std::vector<PrimitiveSubMeshInfo>& PrimitiveSceneInfo::SubMeshInfos( ) const
{
	return m_subMeshInfos;
}

std::vector<PrimitiveSubMesh>& PrimitiveSceneInfo::SubMeshs( )
{
	return m_subMeshs;
}

const std::vector<PrimitiveSubMesh>& PrimitiveSceneInfo::SubMeshs( ) const
{
	return m_subMeshs;
}

DrawSnapshot& PrimitiveSceneInfo::CachedDrawSnapshot( std::size_t snapshotIndex )
{
	const CachedDrawSnapshotInfo& cachedDrawSnapshotInfo = m_cachedDrawSnapshotInfos[snapshotIndex];

	return m_scene.CachedSnapshots( )[cachedDrawSnapshotInfo.m_snapshotIndex];
}

PrimitiveSceneInfo::PrimitiveSceneInfo( PrimitiveComponent* component, Scene& scene ) : m_sceneProxy( component->m_sceneProxy ), m_scene( scene )
{
}

void PrimitiveSceneInfo::CacheDrawSnapshot( )
{
	auto& viewConstant = m_scene.SceneViewConstant( );

	for ( std::size_t i = 0; i < m_subMeshs.size( ); ++i )
	{
		const PrimitiveSubMesh& subMesh = m_subMeshs[i];
		PrimitiveSubMeshInfo& subMeshInfo = m_subMeshInfos[i];

		std::optional<DrawSnapshot> snapshot = m_sceneProxy->TakeSnapshot( subMesh.Lod( ), subMesh.SectionIndex( ), viewConstant );

		if ( snapshot )
		{
			CachedDrawSnapshotInfo cachedDrawSnapshotInfo;
			cachedDrawSnapshotInfo.m_snapshotIndex = m_scene.AddCachedDrawSnapshot( snapshot.value( ) );

			subMeshInfo.m_snapshotInfoBase = m_cachedDrawSnapshotInfos.size( );
			m_cachedDrawSnapshotInfos.emplace_back( cachedDrawSnapshotInfo );
		}
	}
}

void PrimitiveSceneInfo::RemoveCachedDrawSnapshot( )
{
	for ( CachedDrawSnapshotInfo& cachedDrawSnapshotInfo : m_cachedDrawSnapshotInfos )
	{
		m_scene.RemoveCachedDrawSnapshot( cachedDrawSnapshotInfo.m_snapshotIndex );
	}
}
