#include "Query.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<GpuTimer> GpuTimer::Create()
	{
		return GetInterface<IResourceManager>()->CreateGpuTimer();
	}
}