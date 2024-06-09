#pragma once

#include "GuideTypes.h"

namespace logic
{
	class World;
}

namespace rendercore
{
	class IScene;
	class RenderViewGroup;
	class Viewport;

	class IRenderCore
	{
	public:
		virtual bool BootUp() = 0;
		virtual bool IsReady() const = 0;

		virtual void HandleDeviceLost() = 0;
		virtual void AppSizeChanged() = 0;

		virtual IScene* CreateScene( logic::World& world ) = 0;
		virtual void RemoveScene( IScene* scene ) = 0;

		virtual void BeginFrameRendering( Canvas& canvas ) = 0;
		virtual void BeginRenderingViewGroup( RenderViewGroup& renderViewGroup ) = 0;
		virtual void EndFrameRendering( Canvas& canvas ) = 0;

		virtual void GetRawHitProxyData( Viewport& viewport, std::vector<Color>& outHitProxyData ) = 0;

		virtual ~IRenderCore() = default;

	protected:
		IRenderCore() = default;
	};

	Owner<IRenderCore*> CreateRenderCore();
	void DestroyRenderCore( Owner<IRenderCore*> pRenderCore );
}
