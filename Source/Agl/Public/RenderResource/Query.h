#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

namespace agl
{
	class ICommandListBase;

	class Query : public DeviceDependantResource
	{
	public:
		virtual void Begin( ICommandListBase& commandList ) = 0;
		virtual void End( ICommandListBase& commandList ) = 0;
	};

	class GpuTimer : public Query
	{
	public:
		AGL_DLL static RefHandle<GpuTimer> Create();

		virtual double GetDuration() = 0;
	};
}