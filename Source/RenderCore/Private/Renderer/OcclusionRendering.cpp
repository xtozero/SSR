#include "OcclusionRendering.h"

#include "Physics/CollideNarrow.h"
#include "Physics/Frustum.h"
#include "Physics/ICollider.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "SceneRenderer.h"
#include "TaskScheduler.h"
#include "VertexCollection.h"

namespace
{
	constexpr int32 NumOcclusionBoxVertex = 36;

	int32 GetCurrentOcclusionTestIndex( uint64 occlusionFrame )
	{
		return occlusionFrame % rendercore::MaxOcclusionQueryLatency;
	}

	void BuildOcclusionBox( void* dest, const BoxSphereBounds& bounds )
	{
		Vector min = bounds.Origin() - bounds.HalfSize();
		Vector max = bounds.Origin() + bounds.HalfSize();

		auto vertex = static_cast<Vector*>( dest );

		// front
		*( vertex++ ) = Vector( min.x, min.y, min.z );
		*( vertex++ ) = Vector( min.x, max.y, min.z );
		*( vertex++ ) = Vector( max.x, max.y, min.z );
		*( vertex++ ) = Vector( min.x, min.y, min.z );
		*( vertex++ ) = Vector( max.x, max.y, min.z );
		*( vertex++ ) = Vector( max.x, min.y, min.z );

		// back
		*( vertex++ ) = Vector( max.x, min.y, max.z );
		*( vertex++ ) = Vector( max.x, max.y, max.z );
		*( vertex++ ) = Vector( min.x, max.y, max.z );
		*( vertex++ ) = Vector( max.x, min.y, max.z );
		*( vertex++ ) = Vector( min.x, max.y, max.z );
		*( vertex++ ) = Vector( min.x, min.y, max.z );

		// left
		*( vertex++ ) = Vector( min.x, min.y, max.z );
		*( vertex++ ) = Vector( min.x, max.y, max.z );
		*( vertex++ ) = Vector( min.x, max.y, min.z );
		*( vertex++ ) = Vector( min.x, min.y, max.z );
		*( vertex++ ) = Vector( min.x, max.y, min.z );
		*( vertex++ ) = Vector( min.x, min.y, min.z );

		// right
		*( vertex++ ) = Vector( max.x, min.y, min.z );
		*( vertex++ ) = Vector( max.x, max.y, min.z );
		*( vertex++ ) = Vector( max.x, max.y, max.z );
		*( vertex++ ) = Vector( max.x, min.y, min.z );
		*( vertex++ ) = Vector( max.x, max.y, max.z );
		*( vertex++ ) = Vector( max.x, min.y, max.z );

		// top
		*( vertex++ ) = Vector( min.x, max.y, min.z );
		*( vertex++ ) = Vector( min.x, max.y, max.z );
		*( vertex++ ) = Vector( max.x, max.y, max.z );
		*( vertex++ ) = Vector( min.x, max.y, min.z );
		*( vertex++ ) = Vector( max.x, max.y, max.z );
		*( vertex++ ) = Vector( max.x, max.y, min.z );

		// bottom
		*( vertex++ ) = Vector( min.x, min.y, max.z );
		*( vertex++ ) = Vector( min.x, min.y, min.z );
		*( vertex++ ) = Vector( max.x, min.y, min.z );
		*( vertex++ ) = Vector( min.x, min.y, max.z );
		*( vertex++ ) = Vector( max.x, min.y, min.z );
		*( vertex++ ) = Vector( max.x, min.y, max.z );
	}
}

namespace rendercore
{
	class OcclusionVS final : public GlobalShaderCommon<VertexShader, OcclusionVS>
	{
	};

	REGISTER_GLOBAL_SHADER( OcclusionVS, "./Assets/Shaders/VS_Occlusion.asset" );

	DrawSnapshot BuildOcclusionDrawSnapshot( const GlobalDynamicVertexBuffer::AllocationInfo& allcationInfo )
	{
		OcclusionVS occlusionVS;
		VertexStreamLayout vertexlayout;
		vertexlayout.AddLayout( "POSITION", 0, agl::ResourceFormat::R32G32B32_FLOAT, 0, false, 0, 0 );

		RasterizerOption occlusionPassRasterizerOption;
		occlusionPassRasterizerOption.m_cullMode = agl::CullMode::None;
		occlusionPassRasterizerOption.m_scissorEnable = true;

		DepthStencilOption occlusionPassDepthOption;
		occlusionPassDepthOption.m_depth.m_depthFunc = agl::ComparisonFunc::Less;
		occlusionPassDepthOption.m_depth.m_writeDepth = false;

		DrawSnapshot snapshot;
		snapshot.m_vertexStream.Bind( allcationInfo.m_buffer, 0, sizeof( Vector ), allcationInfo.m_offset );
		snapshot.m_primitiveIdSlot = -1;

		snapshot.m_pipelineState.m_shaderState.m_vertexLayout = GraphicsInterface().FindOrCreate( *occlusionVS, vertexlayout );
		snapshot.m_pipelineState.m_shaderState.m_vertexShader = occlusionVS;

		snapshot.m_pipelineState.m_rasterizerState = GraphicsInterface().FindOrCreate( occlusionPassRasterizerOption );
		snapshot.m_pipelineState.m_depthStencilState = GraphicsInterface().FindOrCreate( occlusionPassDepthOption );

		snapshot.m_pipelineState.m_primitive = agl::ResourcePrimitive::Trianglelist;

		auto initializer = CreateShaderBindingsInitializer( snapshot.m_pipelineState.m_shaderState );
		snapshot.m_shaderBindings.Initialize( initializer );

		snapshot.m_count = NumOcclusionBoxVertex;

		PreparePipelineStateObject( snapshot );

		return snapshot;
	}

	void OcclusionQueryAllocator::Prepare( uint64 currentOcclusionFrame )
	{
		assert( IsInRenderThread() );
		m_currentOcclusionFrame = currentOcclusionFrame;
		int32 poolIndex = GetCurrentOcclusionTestIndex( m_currentOcclusionFrame );

		OcclusionQueryPool& occlusionTestList = m_occlusionQueryPool[poolIndex];
		occlusionTestList.m_firstFreeIndex = 0;
	}

	agl::OcclusionQuery* OcclusionQueryAllocator::Allocate()
	{
		assert( IsInRenderThread() );
		int32 poolIndex = GetCurrentOcclusionTestIndex( m_currentOcclusionFrame );

		OcclusionQueryPool& occlusionTestList = m_occlusionQueryPool[poolIndex];
		if ( occlusionTestList.m_firstFreeIndex >= occlusionTestList.m_queries.size() )
		{
			auto occlusionTest = agl::OcclusionQuery::Create();
			occlusionTest->Init();
			occlusionTestList.m_queries.emplace_back( std::move( occlusionTest ) );
		}

		return occlusionTestList.m_queries[occlusionTestList.m_firstFreeIndex++].Get();
	}

	agl::OcclusionQuery* OcclusionTestData::GetLatestOuery( uint64 currentOcclusionFrame )
	{
		agl::OcclusionQuery* properQuery = nullptr;
		int64 minOcclusionFrameDiff = MaxOcclusionQueryLatency;

		for ( int32 i = 0; i < MaxOcclusionQueryLatency; ++i )
		{
			int32 poolIndex = GetCurrentOcclusionTestIndex( currentOcclusionFrame + i );
			if ( m_occlusionQuery[poolIndex] == nullptr )
			{
				continue;
			}

			if ( m_occlusionQuery[poolIndex]->IsDataReady() == false )
			{
				continue;
			}

			int64 occlusionFrameDiff = currentOcclusionFrame - m_frameCounter[poolIndex];
			if ( occlusionFrameDiff < minOcclusionFrameDiff )
			{
				properQuery = m_occlusionQuery[poolIndex];
				minOcclusionFrameDiff = occlusionFrameDiff;
			}
		}

		return properQuery;
	}

	void OcclusionTestData::BeginQuery( RenderViewState& viewState )
	{
		int32 poolIndex = GetCurrentOcclusionTestIndex( viewState.m_occlusionFrameCounter );
		m_occlusionQuery[poolIndex] = viewState.m_occlutionTestPool.Allocate();

		m_frameCounter[poolIndex] = viewState.m_occlusionFrameCounter;

		auto commandList = GetCommandList();
		commandList.BeginQuery( m_occlusionQuery[poolIndex] );
	}

	void OcclusionTestData::EndQuery( RenderViewState& viewState )
	{
		int32 poolIndex = GetCurrentOcclusionTestIndex( viewState.m_occlusionFrameCounter );
		assert( m_occlusionQuery[poolIndex] != nullptr );

		auto commandList = GetCommandList();
		commandList.EndQuery( m_occlusionQuery[poolIndex] );
	}

	void OcclusionTestData::UpdateOccludeState( bool occluded )
	{
		m_wasOccluded = occluded;
	}

	bool OcclusionTestData::WasOccluded() const
	{
		return m_wasOccluded;
	}

	void OcclusionCull( const IScene& scene, GlobalDynamicVertexBuffer& outDynamicVertexBuffer, RenderViewInfo& outViewInfo, RenderThreadFrameData<OcclusionRenderData>& outOcclusionRenderData )
	{
		assert( outViewInfo.m_state != nullptr );
		RenderViewState& viewState = *outViewInfo.m_state;

		Plane nearPlane = Frustum( outViewInfo.m_viewProjMatrix ).GetPlane( Frustum::PlaneDir::Near );

		const auto& occlusionBounds = scene.PrimitiveOcclusionBounds();

		const auto& primitives = scene.Primitives();
		for ( auto primitive : primitives )
		{
			uint32 primitiveId = primitive->PrimitiveId();
			
			if ( outViewInfo.m_visibilityMap[primitiveId] == false )
			{
				continue;
			}

			auto found = viewState.m_occlusionTestDataSet.find( primitiveId );
			if ( found == std::end( viewState.m_occlusionTestDataSet ) )
			{
				found = viewState.m_occlusionTestDataSet.emplace( primitiveId, OcclusionTestData() ).first;
				continue;
			}

			OcclusionTestData& occlusionTestData = found->second;
			bool occluded = occlusionTestData.WasOccluded();

			agl::OcclusionQuery* occlusionQuery = occlusionTestData.GetLatestOuery( viewState.m_occlusionFrameCounter );
			if ( occlusionQuery != nullptr )
			{
				occluded = ( occlusionQuery->GetNumSamplePassed() == 0 );
			}
			else
			{
				occluded = false;
			}

			const BoxSphereBounds& bounds = occlusionBounds[primitiveId];
			if ( BoxAndPlane( bounds.Origin(), bounds.HalfSize(), nearPlane ) == CollisionResult::Intersection )
			{
				occluded = false;
			}

			occlusionTestData.UpdateOccludeState( occluded );
			outViewInfo.m_visibilityMap[primitiveId] = (occluded == false);

			OcclusionRenderData& occlusionRenderData = outOcclusionRenderData.emplace_back( primitiveId, outDynamicVertexBuffer.Allocate( sizeof( Vector ) * NumOcclusionBoxVertex ) );

			BuildOcclusionBox( occlusionRenderData.m_allcationInfo.m_lockedMemory, bounds );
		}
	}

	void DoRenderOcclusionTest( RenderingShaderResource& resources, RenderViewInfo& viewInfo, const RenderThreadFrameData<OcclusionRenderData>& occlusionRenderData )
	{
		if ( occlusionRenderData.empty() )
		{
			return;
		}

		assert( viewInfo.m_state != nullptr );
		RenderViewState& viewState = *viewInfo.m_state;

		auto commandList = GetCommandList();

		for ( const OcclusionRenderData& renderData : occlusionRenderData )
		{
			auto found = viewState.m_occlusionTestDataSet.find( renderData.m_primitiveId );
			if ( found == std::end( viewState.m_occlusionTestDataSet ) )
			{
				continue;
			}

			found->second.BeginQuery( viewState );

			auto snapshot = BuildOcclusionDrawSnapshot( renderData.m_allcationInfo );
			resources.BindResources( snapshot.m_pipelineState.m_shaderState, snapshot.m_shaderBindings );
			
			VisibleDrawSnapshot visibleSnapshot = {
				.m_primitiveId = 0,
				.m_primitiveIdOffset = 0,
				.m_numInstance = 1,
				.m_snapshotBucketId = -1,
				.m_drawSnapshot = &snapshot,
			};

			VertexBuffer emptyPrimitiveID;
			CommitDrawSnapshot( commandList, visibleSnapshot, emptyPrimitiveID );

			found->second.EndQuery( viewState );
		}
	}
}
