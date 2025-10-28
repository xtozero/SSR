#include "Scene/PrimitiveSceneInfo.h"

#include "Components/PrimitiveComponent.h"
#include "Config/DefaultAppConfig.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "MaterialResource.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "Scene/Scene.h"
#include "VertexCollection.h"

namespace rendercore
{
	void PrimitiveSubMeshInfo::OnDrawSnapshotAdded( RenderPassType passType )
	{
		m_passTypeMask |= 1 << static_cast<uint32>( passType );
	}

	void PrimitiveSubMeshInfo::SetDrawSnapshotInfoBase( uint32 snapshotInfoBase )
	{
		m_drawSnapshotInfoBase = snapshotInfoBase;
	}

	void PrimitiveSubMeshInfo::SetShadingSnapshotId( int32 shadingSnapshotId )
	{
		m_shadingSnapshotId = shadingSnapshotId;
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
		CacheShadingSnapshot();

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
		RemoveCachedShadingSnapshot();

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

	DrawSnapshot& PrimitiveSceneInfo::GetCachedDrawSnapshot( uint32 snapshotIndex )
	{
		const CachedDrawSnapshotInfo& cachedDrawSnapshotInfo = m_cachedDrawSnapshotInfos[snapshotIndex];

		return m_scene.GetCachedDrawSnapshots( cachedDrawSnapshotInfo.m_renderPass )[cachedDrawSnapshotInfo.m_snapshotIndex];
	}

	ShadingSnapshot& PrimitiveSceneInfo::GetShadingSnapshot( int32 shadingSnapshotId )
	{
		return m_scene.GetCachedShadingSnapshot( shadingSnapshotId );
	}

	HitProxyId PrimitiveSceneInfo::GetHitProxyId() const
	{
		if ( m_hitProxy.Get() )
		{
			return m_hitProxy->GetId();
		}

		return HitProxyId();
	}

	PrimitiveSceneInfo::PrimitiveSceneInfo( logic::PrimitiveComponent* component, Scene& scene ) : m_scene( scene ), m_sceneProxy( component->m_sceneProxy )
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
			subMeshInfo.SetDrawSnapshotInfoBase( static_cast<uint32>( m_cachedDrawSnapshotInfos.size() ) );

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

	void PrimitiveSceneInfo::CacheShadingSnapshot()
	{
		for ( size_t i = 0; i < m_subMeshs.size(); ++i )
		{
			const PrimitiveSubMesh& subMesh = m_subMeshs[i];
			if (subMesh.m_material == nullptr)
			{
				continue;
			}

			MaterialResource& material = *subMesh.m_material;
			StaticShaderSwitches csSwitches = material.GetShaderSwitches( agl::ShaderType::CS );

			if ( DefaultRenderCore::IsRSMsEnabled() )
			{
				csSwitches.On( StaticName( "EnableRSMs" ), 1 );
			}

			if ( DefaultRenderCore::UseIrradianceMapSH() )
			{
				csSwitches.On( StaticName( "UseIrradianceMapSH" ), 1 );
			}

			if ( agl::DefaultAgl::SupportsBindless() )
			{
				csSwitches.On( StaticName( "SupportsBindless" ), 1 );
			}

			auto computeShader = material.GetComputeShader( &csSwitches );
			if ( computeShader == nullptr )
			{
				continue;
			}

			ShadingSnapshot snapshot = {
				.m_computeShader = computeShader,
				.m_pso = PrepareComputePipelineState( computeShader )
			};

			agl::ShaderBindingsInitializer initializer;
			initializer[agl::ShaderType::CS] = &computeShader->ParameterInfo();
			snapshot.m_shaderBindings.Initialize( initializer );

			const agl::ShaderParameterMap& shaderParameterMap = computeShader->ParameterMap();
			agl::SingleShaderBindings singleShaderBindings = snapshot.m_shaderBindings.GetSingleShaderBindings( agl::ShaderType::CS );
			subMesh.m_vertexCollection->Bind( shaderParameterMap, singleShaderBindings );

			agl::ShaderParameter indexBufferParam = shaderParameterMap.GetParameter( StaticName( "Indices" ) );
			singleShaderBindings.AddSRV( indexBufferParam, subMesh.m_indexBuffer->Resource()->SRV() );

			material.TakeSnapshot( snapshot );

			snapshot.m_startIndexLocation = subMesh.m_startLocation;
			snapshot.m_baseVertexLocation = subMesh.m_baseVertexLocation;

			int32 cachedShadingSnapshotId = m_scene.AddCachedShadingSnapshot( snapshot );
			m_cachedShadingSnapshotIds.emplace_back( cachedShadingSnapshotId );

			PrimitiveSubMeshInfo& subMeshInfo = m_subMeshInfos[i];
			subMeshInfo.SetShadingSnapshotId( cachedShadingSnapshotId );
		}
	}

	void PrimitiveSceneInfo::RemoveCachedShadingSnapshot()
	{
		for ( int32 cachedShadingSnapshotId : m_cachedShadingSnapshotIds )
		{
			m_scene.RemoveCachedShadingSnapshot( cachedShadingSnapshotId );
		}

		m_cachedShadingSnapshotIds.clear();
	}

	std::optional<uint32> PrimitiveSubMeshInfo::GetCachedDrawSnapshotInfoIndex( RenderPassType passType ) const
	{
		uint32 iPassType = static_cast<uint32>( passType );
		if ( ( m_passTypeMask & ( 1 << iPassType ) ) == 0 )
		{
			return {};
		}

		uint32 snapshotInfoIndex = m_drawSnapshotInfoBase;
		for ( uint32 i = 0; i < iPassType; ++i )
		{
			if ( ( m_passTypeMask & ( 1 << i ) ) > 0 )
			{
				++snapshotInfoIndex;
			}
		}

		return snapshotInfoIndex;
	}

	int32 PrimitiveSubMeshInfo::GetShadingSnapshotId() const
	{
		return m_shadingSnapshotId;
	}

	FullScreenQuadDrawInfo::FullScreenQuadDrawInfo()
	{
		m_count = 3;
	}
}
