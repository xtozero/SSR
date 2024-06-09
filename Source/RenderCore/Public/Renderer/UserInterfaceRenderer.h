#pragma once

#include "GuideTypes.h"

namespace rendercore
{
	class Canvas;
	class RenderViewGroup;

	class UserInterfaceRenderer
	{
	public:
		virtual bool BootUp() = 0;
		virtual void Render( Canvas& canvas ) = 0;

		virtual void UpdateUIDrawInfo() = 0;

		virtual ~UserInterfaceRenderer() = default;
	};

	Owner<UserInterfaceRenderer*> CreateUserInterfaceRenderer();
	void DestroyUserInterfaceRenderer( Owner<UserInterfaceRenderer*> uiRenderer );
}