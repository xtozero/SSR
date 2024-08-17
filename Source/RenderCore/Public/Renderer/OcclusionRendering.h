#pragma once

#include "Query.h"
#include "RefHandle.h"
#include "SizedTypes.h"
#include "VertexBuffer.h"

class BoxSphereBounds;

namespace rendercore
{
	class IScene;
	class RenderingShaderResource;

	struct RenderViewInfo;
	struct RenderViewState;

	static constexpr int32 MaxOcclusionQueryLatency = 5;

	class OcclusionQueryAllocator
	{
	public:
		void Prepare( uint64 currentOcclusionFrame );
		agl::OcclusionQuery* Allocate();

	private:
		struct OcclusionQueryPool
		{
			std::vector<RefHandle<agl::OcclusionQuery>> m_queries;
			size_t m_firstFreeIndex = 0;
		};

		OcclusionQueryPool m_occlusionQueryPool[MaxOcclusionQueryLatency];
		uint64 m_currentOcclusionFrame = 0;
	};

	class OcclusionTestData
	{
	public:
		agl::OcclusionQuery* GetLatestOuery( uint64 currentOcclusionFrame );

		void BeginQuery( RenderViewState& viewState );
		void EndQuery( RenderViewState& viewState );
		
		void UpdateOccludeState( bool occluded );
		bool WasOccluded() const;

	private:
		agl::OcclusionQuery* m_occlusionQuery[MaxOcclusionQueryLatency] = {};
		uint64 m_frameCounter[MaxOcclusionQueryLatency] = {};
		bool m_wasOccluded = false;
	};
	
	struct OcclusionRenderData
	{
		OcclusionRenderData( uint32 primitiveId, const GlobalDynamicVertexBuffer::AllocationInfo& allcationInfo )
			: m_primitiveId( primitiveId )
			, m_allcationInfo( allcationInfo )
		{}
		OcclusionRenderData() = default;

		uint32 m_primitiveId = 0;
		GlobalDynamicVertexBuffer::AllocationInfo m_allcationInfo;
	};

	void OcclusionCull( const IScene& scene, GlobalDynamicVertexBuffer& outDynamicVertexBuffer, RenderViewInfo& outViewInfo, RenderThreadFrameData<OcclusionRenderData>& outOcclusionRenderData );
	void DoRenderOcclusionTest( RenderingShaderResource& resources, RenderViewInfo& viewInfo, const RenderThreadFrameData<OcclusionRenderData>& occlusionRenderData );
}
