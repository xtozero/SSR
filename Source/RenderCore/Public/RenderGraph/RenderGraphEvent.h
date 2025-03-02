#pragma once

#include "GPUProfiler.h"

namespace rendercore
{
	class RenderGraph;
	class RenderGraphPass;

	template <typename T>
	struct RenderGraphEvent
	{
		using ThisType = RenderGraphEvent<T>;

		ThisType* m_parent = nullptr;

		RenderGraphPass* m_firstPass = nullptr;
		RenderGraphPass* m_lastPass = nullptr;

		T* m_eventData = nullptr;
	};

	using GpuProfileRenderGraphEvent = RenderGraphEvent<GpuProfileData>;
	using PipelineStateRenderGraphEvent = RenderGraphEvent<PipelineStatData>;

	class RenderGraphScopedGpuProfile
	{
	public:
		RenderGraphScopedGpuProfile( RenderGraph& renderGraph, GpuProfileData& gpuProfileData );
		~RenderGraphScopedGpuProfile();

	private:
		RenderGraph& m_renderGraph;
	};

	class RenderGraphScopedPipelineStat
	{
	public:
		RenderGraphScopedPipelineStat( RenderGraph& renderGraph, PipelineStatData& pipelineStatData );
		~RenderGraphScopedPipelineStat();

	private:
		RenderGraph& m_renderGraph;
	};

	void BeginGpuProfileEvent( ComputeCommandList& commandList, RenderGraphPass& pass, GpuProfileRenderGraphEvent* event );
	void EndGpuProfileEvent( ComputeCommandList& commandList, RenderGraphPass& pass, GpuProfileRenderGraphEvent* event );

	void BeginPipelineStatEvent( ComputeCommandList& commandList, RenderGraphPass& pass, PipelineStateRenderGraphEvent* event );
	void EndPipelineStatEvent( ComputeCommandList& commandList, RenderGraphPass& pass, PipelineStateRenderGraphEvent* event );

#if ENABLE_GPU_PROFILE
#define GPU_PROFILE_EVENT( renderGraph, name ) \
	static GpuProfileData GpuProfileData_##name( #name ); \
	static RegisterGpuProfileData RegisterGpuProfileData_##name( GpuProfileData_##name ); \
	RenderGraphScopedGpuProfile ScopedGpuProfile_##name( renderGraph, GpuProfileData_##name );
#else
#define GPU_PROFILE_EVENT( renderGraph, name )
#endif

#if ENABLE_PIPELINE_STATISTICS
#define PIPELINE_STAT_EVENT( renderGraph, name ) \
	static PipelineStatData PipelineStatData_##name( #name ); \
	static RegisterPipelineStatData RegisterPipelineStatData_##name( PipelineStatData_##name ); \
	RenderGraphScopedPipelineStat ScopedPipelineStat_##name( renderGraph, PipelineStatData_##name );
#else
#define PIPELINE_STAT_EVENT( renderGraph, name )
#endif
}