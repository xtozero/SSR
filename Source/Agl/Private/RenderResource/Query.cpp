#include "Query.h"

#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<GpuTimer> GpuTimer::Create()
	{
		return GetInterface<IResourceManager>()->CreateGpuTimer();
	}

	RefHandle<OcclusionQuery> OcclusionQuery::Create()
	{
		return GetInterface<IResourceManager>()->CreateOcclusionQuery();
	}

	RefHandle<PipelineStatistics> PipelineStatistics::Create()
	{
		return GetInterface<IResourceManager>()->CreatePipelineStatistics();
	}
}