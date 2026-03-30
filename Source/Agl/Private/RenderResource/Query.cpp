#include "Query.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<GpuTimer> GpuTimer::Create()
	{
		auto newGpuTimer = GetInterface<IResourceManager>()->CreateGpuTimer();
		EnqueueRenderTask(
			[gpuTimer = newGpuTimer]()
			{
				gpuTimer->Init();
			} );

		return newGpuTimer;
	}

	RefHandle<OcclusionQuery> OcclusionQuery::Create()
	{
		auto newQuery = GetInterface<IResourceManager>()->CreateOcclusionQuery();
		EnqueueRenderTask(
			[query = newQuery]()
			{
				query->Init();
			} );

		return newQuery;
	}

	RefHandle<PipelineStatistics> PipelineStatistics::Create()
	{
		auto newPipelineStatistics = GetInterface<IResourceManager>()->CreatePipelineStatistics();
		EnqueueRenderTask(
			[pipelineStatistics = newPipelineStatistics]()
			{
				pipelineStatistics->Init();
			} );

		return newPipelineStatistics;
	}
}