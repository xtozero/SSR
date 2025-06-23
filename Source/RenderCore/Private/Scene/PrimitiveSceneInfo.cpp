#include "Scene/PrimitiveSceneInfo.h"

#include "Components/PrimitiveComponent.h"
#include "Config/DefaultAppConfig.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "Scene/Scene.h"

namespace rendercore
{
	void PrimitiveSubMeshInfo::OnDrawSnapshotAdded( RenderPassType passType )
	{
		m_passTypeMask |= 1 << static_cast<uint32>( passType );
	}

	uint32& PrimitiveSubMeshInfo::SnapshotInfoBase()
	{
		return m_snapshotInfoBase;
	}

	uint32 PrimitiveSubMeshInfo::SnapshotInfoBase() const
	{
		return m_snapshotInfoBase;
	}

	PrimitiveProxy*& PrimitiveSceneInfo::Proxy()
	{
		return m_sceneProxy;
	}

	const PrimitiveProxy* PrimitiveSceneInfo::Proxy() const
	{
		return m_sceneProxy;
	}

	uint32& PrimitiveSceneInfo::PrimitiveId()
	{
		return m_primitiveId;
	}

	uint32 PrimitiveSceneInfo::PrimitiveId() const
	{
		return m_primitiveId;
	}

	void PrimitiveSceneInfo::AddToScene()
	{
		const BoxSphereBounds& bounds = m_sceneProxy->Bounds();
		m_scene.PrimitiveBounds()[m_primitiveId] = bounds;

		BoxSphereBounds occlusionBounds = bounds;
		constexpr float OcclusionPadding = 1.f;
		occlusionBounds.HalfSize().x += OcclusionPadding;
		occlusionBounds.HalfSize().y += OcclusionPadding;
		occlusionBounds.HalfSize().z += OcclusionPadding;
		occlusionBounds.Radius() += OcclusionPadding;
		m_scene.PrimitiveOcclusionBounds()[m_primitiveId] = occlusionBounds;

		m_sceneProxy->PrepareSubMeshs();

		CacheDrawSnapshot();

		for ( LightSceneInfo* light : m_scene.Lights() )
		{
			if ( light->Proxy()->CastShadow() == false )
			{
				continue;
			}

			light->AddPrimitiveIntersectionInfo( *this );
		}
	}

	void PrimitiveSceneInfo::RemoveFromScene()
	{
		m_subMeshInfos.clear();
		m_subMeshs.clear();

		RemoveCachedDrawSnapshot();

		for ( LightSceneInfo* light : m_scene.Lights() )
		{
			if ( light->Proxy()->CastShadow() == false )
			{
				continue;
			}

			light->RemovePrimitiveIntersectionInfo( *this );
		}

		assert( m_lightList.Size() == 0 );
	}

	PrimitiveSubMesh& PrimitiveSceneInfo::AddSubMesh()
	{
		m_subMeshInfos.emplace_back();
		PrimitiveSubMesh& subMesh = m_subMeshs.emplace_back();
		return subMesh;
	}

	std::vector<PrimitiveSubMeshInfo>& PrimitiveSceneInfo::SubMeshInfos()
	{
		return m_subMeshInfos;
	}

	const std::vector<PrimitiveSubMeshInfo>& PrimitiveSceneInfo::SubMeshInfos() const
	{
		return m_subMeshInfos;
	}

	std::vector<PrimitiveSubMesh>& PrimitiveSceneInfo::SubMeshs()
	{
		return m_subMeshs;
	}

	const std::vector<PrimitiveSubMesh>& PrimitiveSceneInfo::SubMeshs() const
	{
		return m_subMeshs;
	}

	SparseArray<LightIntersectionInfo>& PrimitiveSceneInfo::Lights()
	{
		return m_lightList;
	}

	const SparseArray<LightIntersectionInfo>& PrimitiveSceneInfo::Lights() const
	{
		return m_lightList;
	}

	const CachedDrawSnapshotInfo& PrimitiveSceneInfo::GetCachedDrawSnapshotInfo( uint32 snapshotInfoBase )
	{
		return m_cachedDrawSnapshotInfos[snapshotInfoBase];
	}

	DrawSnapshot& PrimitiveSceneInfo::CachedDrawSnapshot( uint32 snapshotIndex )
	{
		const CachedDrawSnapshotInfo& cachedDrawSnapshotInfo = m_cachedDrawSnapshotInfos[snapshotIndex];

		return m_scene.CachedSnapshots( cachedDrawSnapshotInfo.m_renderPass )[cachedDrawSnapshotInfo.m_snapshotIndex];
	}

	HitProxyId PrimitiveSceneInfo::GetHitProxyId() const
	{
		if ( m_hitProxy.Get() )
		{
			return m_hitProxy->GetId();
		}

		return HitProxyId();
	}

	PrimitiveSceneInfo::PrimitiveSceneInfo( logic::PrimitiveComponent* component, Scene& scene ) : m_sceneProxy( component->m_sceneProxy ), m_scene( scene )
	{
		if ( engine::DefaultApp::IsEditor() )
		{
			m_hitProxy = m_sceneProxy->CreateHitProxy( component );
		}
	}

	void PrimitiveSceneInfo::CacheDrawSnapshot()
	{
		for ( size_t i = 0; i < m_subMeshs.size(); ++i )
		{
			const PrimitiveSubMesh& subMesh = m_subMeshs[i];
			PrimitiveSubMeshInfo& subMeshInfo = m_subMeshInfos[i];
			subMeshInfo.SnapshotInfoBase() = static_cast<uint32>( m_cachedDrawSnapshotInfos.size() );

			for ( uint32 j = 0; j < static_cast<uint32>( RenderPassType::Count ); ++j )
			{
				auto passType = static_cast<RenderPassType>( j );
				IPassProcessor* processor = PassProcessorManager::GetPassProcessor( passType );
				if ( processor == nullptr )
				{
					continue;
				}

				std::optional<DrawSnapshot> snapshot = processor->Process( subMesh );

				if ( snapshot )
				{
					CachedDrawSnapshotInfo cachedDrawSnapshotInfo = m_scene.AddCachedDrawSnapshot( passType, snapshot.value() );

					m_cachedDrawSnapshotInfos.emplace_back( cachedDrawSnapshotInfo );
					subMeshInfo.OnDrawSnapshotAdded( passType );
				}
			}
		}
	}

	void PrimitiveSceneInfo::RemoveCachedDrawSnapshot()
	{
		for ( CachedDrawSnapshotInfo& cachedDrawSnapshotInfo : m_cachedDrawSnapshotInfos )
		{
			m_scene.RemoveCachedDrawSnapshot( cachedDrawSnapshotInfo );
		}

		m_cachedDrawSnapshotInfos.clear();
	}

	std::optional<uint32> PrimitiveSubMeshInfo::GetCachedDrawSnapshotInfoIndex( RenderPassType passType ) const
	{
		uint32 iPassType = static_cast<uint32>( passType );
		if ( ( m_passTypeMask & ( 1 << iPassType ) ) == 0 )
		{
			return {};
		}

		uint32 snapshotInfoIndex = m_snapshotInfoBase;
		for ( uint32 i = 0; i < iPassType; ++i )
		{
			if ( ( m_passTypeMask & ( 1 << i ) ) > 0 )
			{
				++snapshotInfoIndex;
			}
		}

		return snapshotInfoIndex;
	}

	FullScreenQuadDrawInfo::FullScreenQuadDrawInfo()
	{
		m_count = 3;
	}
}
