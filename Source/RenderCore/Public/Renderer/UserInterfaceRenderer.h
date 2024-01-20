#pragma once

#include "GuideTypes.h"

namespace rendercore
{
	class RenderViewGroup;

	class UserInterfaceRenderer
	{
	public:
		virtual bool BootUp() = 0;
		virtual void Render( RenderViewGroup& renderViewGroup ) = 0;

		virtual void UpdateUIDrawInfo() = 0;

		virtual ~UserInterfaceRenderer() = default;
	};

	Owner<UserInterfaceRenderer*> CreateUserInterfaceRenderer();
	void DestroyUserInterfaceRenderer( Owner<UserInterfaceRenderer*> uiRenderer );
}