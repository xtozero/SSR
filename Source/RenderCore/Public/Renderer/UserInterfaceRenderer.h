#pragma once

#include "GuideTypes.h"

namespace rendercore
{
	class Canvas;
	class RenderGraph;
	class RenderViewGroup;

	class UserInterfaceRenderer
	{
	public:
		virtual bool BootUp() = 0;

		virtual void Render( RenderGraph& renderGraph ) = 0;

		virtual void UpdateUIDrawInfo( Canvas& canvas ) = 0;

		virtual ~UserInterfaceRenderer() = default;
	};

	Owner<UserInterfaceRenderer*> CreateUserInterfaceRenderer();
	void DestroyUserInterfaceRenderer( Owner<UserInterfaceRenderer*> uiRenderer );
}