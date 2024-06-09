#pragma once

#include "common.h"
#include "GuideTypes.h"
#include "SizedTypes.h"

#include <memory>

namespace engine
{
	class IPlatform;

	class IEngine
	{
	public:
		virtual bool BootUp( IPlatform& platform, const char* argv ) = 0;

		virtual void Run() = 0;

		virtual void ProcessInput() = 0;

		virtual ~IEngine() = default;

	protected:
		virtual bool IsAvailable() = 0;
	};

	namespace SUPPORT_PLATFORM
	{
		struct Window {};
	}

	Owner<IEngine*> CreatePlatformEngine();
	void DestroyPlatformEngine( Owner<IEngine*> pEngine );
}