#include "stdafx.h"
#include "Scene/ShadowInfo.h"

#include "Config/DefaultRenderCoreConfig.h"
#include "Proxies/LightProxy.h"
#include "Proxies/PrimitiveProxy.h"
#include "Renderer/SceneRenderer.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/PrimitiveSceneInfo.h"

#include <utility>

namespace
{
	std::pair<uint32, uint32> CalcShadowmapSize( uint32 quility )
	{
		switch ( quility )
		{
		case 0: // Low
			return { 512, 512 };
			break;
		case 1: // Mid
			return { 1024, 1024 };
			break;
		default: // High
			return { 2048, 2048 };
			break;
		}
	}
}

namespace rendercore
{
	LightType ShadowInfo::GetLightType() const
	{
		return m_lightSceneInfo->Proxy()->GetLightType();
	}

	void ShadowInfo::AddCasterPrimitive( PrimitiveSceneInfo* primitiveSceneInfo )
	{
		m_shadowCasters.emplace_back( primitiveSceneInfo );

		if ( primitiveSceneInfo->SubMeshInfos().size() > 0 )
		{
			AddCachedDrawSnapshotForPass( *primitiveSceneInfo );
		}
		else
		{
			// For Dynamic Mesh
		}
	}

	void ShadowInfo::AddCasterPrimitive( PrimitiveSceneInfo* primitiveSceneInfo, const BoxSphereBounds& viewspaceBounds )
	{
		m_shadowCastersViewSpaceBounds.emplace_back( viewspaceBounds );
		UpdateSubjectNearAndFar( viewspaceBounds );

		AddCasterPrimitive( primitiveSceneInfo );
	}

	void ShadowInfo::AddReceiverPrimitive( PrimitiveSceneInfo* primitiveSceneInfo, const BoxSphereBounds& viewspaceBounds )
	{
		m_shadowReceivers.emplace_back( primitiveSceneInfo );
		m_shadowReceiversViewSpaceBounds.emplace_back( viewspaceBounds );
		UpdateSubjectNearAndFar( viewspaceBounds );
	}

	void ShadowInfo::SetupShadowConstantBuffer()
	{
		ShadowDepthPassParameters params = {};
		LightProperty lightProperty = m_lightSceneInfo->Proxy()->GetLightProperty();
		params.m_lightPosOrDir = ( lightProperty.m_type == LightType::Directional ) ? Vector4( lightProperty.m_direction ) : lightProperty.m_position;
		params.m_slopeBiasScale = 0.2f;
		params.m_constantBias = ( lightProperty.m_type == LightType::Directional ) ? 0.05f : 0.5f;
		params.m_lightIdx = m_lightSceneInfo->IdOnGPU();

		for ( uint32 i = 0; i < CascadeShadowSetting::MAX_CASCADE_NUM; ++i )
		{
			params.m_cascadeFar[i].x = m_cacadeSetting.m_splitDistance[i + 1];
			params.m_shadowViewProjection[i] = m_shadowViewProjections[i].GetTrasposed();
		}

		for ( uint32 i = CascadeShadowSetting::MAX_CASCADE_NUM; i < 6; ++i )
		{
			params.m_shadowViewProjection[i] = m_shadowViewProjections[i].GetTrasposed();
		}

		m_shadowConstantBuffer.Update( params );

		if ( DefaultRenderCore::IsESMsEnabled() )
		{
			if ( m_pESMsConstantBuffer == nullptr )
			{
				m_pESMsConstantBuffer.reset( new TypedConstatBuffer<ESMsParameters>() );
			}

			ESMsParameters esmsParams = {
				.m_esmsParameterC = DefaultRenderCore::ESMsParamC()
			};

			m_pESMsConstantBuffer->Update( esmsParams );
		}
		else
		{
			m_pESMsConstantBuffer = nullptr;
		}
	}

	void ShadowInfo::RenderDepth( SceneRenderer& renderer, RenderingShaderResource& resources )
	{
		if ( m_snapshots.empty() )
		{
			return;
		}

		RenderingShaderResource shadowRenderingResources = resources;

		shadowRenderingResources.AddResource( "ShadowDepthPassParameters", m_shadowConstantBuffer.Resource() );

		// Update invalidated resources
		for ( auto& viewDrawSnapshot : m_snapshots )
		{
			DrawSnapshot& snapshot = *viewDrawSnapshot.m_drawSnapshot;
			GraphicsPipelineState& pipelineState = snapshot.m_pipelineState;

			shadowRenderingResources.BindResources( pipelineState.m_shaderState, snapshot.m_shaderBindings );
		}

		VertexBuffer primitiveIds = renderer.GetPrimitiveIdPool().Alloc( static_cast<uint32>( m_snapshots.size() * sizeof( uint32 ) ) );

		SortDrawSnapshots( m_snapshots, primitiveIds );
		ParallelCommitDrawSnapshot( renderer, m_snapshots, primitiveIds );
	}

	ShadowInfo::ShadowInfo( LightSceneInfo* lightSceneInfo, const RenderView& view ) : m_lightSceneInfo( lightSceneInfo ), m_view( &view )
	{
		auto [width, height] = CalcShadowmapSize( m_lightSceneInfo->GetShadowQuility() );
		m_shadowMapWidth = width;
		m_shadowMapHeight = height;

		size_t numPrimitives = m_lightSceneInfo->Primitives().Size();
		m_shadowCasters.reserve( numPrimitives );
		m_shadowReceivers.reserve( numPrimitives );
	}

	void ShadowInfo::AddCachedDrawSnapshotForPass( PrimitiveSceneInfo& primitiveSceneInfo )
	{
		const std::vector<PrimitiveSubMeshInfo>& subMeshInfos = primitiveSceneInfo.SubMeshInfos();

		PrimitiveProxy* proxy = primitiveSceneInfo.Proxy();

		if ( subMeshInfos.size() > 0 )
		{
			for ( const auto& subMeshInfo : subMeshInfos )
			{
				auto snapshotIndex = subMeshInfo.GetCachedDrawSnapshotInfoIndex( GetShadowDepthRenderPass() );
				if ( snapshotIndex )
				{
					const CachedDrawSnapshotInfo& info = primitiveSceneInfo.GetCachedDrawSnapshotInfo( *snapshotIndex );
					DrawSnapshot& snapshot = primitiveSceneInfo.CachedDrawSnapshot( *snapshotIndex );

					VisibleDrawSnapshot& visibleSnapshot = m_snapshots.emplace_back();
					visibleSnapshot.m_snapshotBucketId = info.m_snapshotBucketId;
					visibleSnapshot.m_drawSnapshot = &snapshot;
					visibleSnapshot.m_primitiveId = proxy->PrimitiveId();
					visibleSnapshot.m_numInstance = 1;
				}
			}
		}
	}

	void ShadowInfo::UpdateSubjectNearAndFar( const BoxSphereBounds& viewspaceBounds )
	{
		Vector min = ( viewspaceBounds.Origin() - viewspaceBounds.HalfSize() );
		Vector max = ( viewspaceBounds.Origin() + viewspaceBounds.HalfSize() );

		m_subjectNear = std::min( m_subjectNear, min.z );
		m_subjectFar = std::max( m_subjectFar, max.z );
	}

	RenderPass ShadowInfo::GetShadowDepthRenderPass() const
	{
		switch ( GetLightType() )
		{
		case LightType::Directional:
			return RenderPass::CascadeShadowDepth;
			break;
		case LightType::Point:
			return RenderPass::PointShadowDepth;
			break;
		case LightType::Spot:
			return RenderPass::PointShadowDepth;
			break;
		}

		assert( false && "GetShadowDepthRenderPass - Unsurpported LightType" );
		return RenderPass::CascadeShadowDepth;
	}
}
