#pragma once

#include "Reflection.h"
#include "SizedTypes.h"

namespace rendercore
{
	class Canvas;
	class HitProxy;
	class RenderViewGroup;
	class Viewport;

	struct RenderViewShowFlags;
}

namespace logic
{
	class World;

	class GameClientViewport
	{
		GENERATE_CLASS_TYPE_INFO( GameClientViewport )

	public:
		void BeginFrameRendering( rendercore::Canvas& canvas );
		LOGIC_DLL void Draw( rendercore::Canvas& canvas, const rendercore::RenderViewShowFlags* showFlags = nullptr );
		void EndFrameRendering( rendercore::Canvas& canvas );

		World& GetWorld();

		void SetViewport( rendercore::Viewport* viewport );
		LOGIC_DLL rendercore::Viewport* GetViewport();

		LOGIC_DLL rendercore::HitProxy* GetHitProxy( uint32 x, uint32 y );

		void AppSizeChanged( const std::pair<uint32, uint32>& newSize );

		LOGIC_DLL void InvalidateHitProxy();

		GameClientViewport( World& world, rendercore::Viewport* viewport ) 
			: m_world( world )
			, m_viewport( viewport ) { }

	private:
		void InitView( rendercore::RenderViewGroup& views );
		const std::vector<Color>& GetRawHitProxyData();

		World& m_world;
		rendercore::Viewport* m_viewport = nullptr;

		bool m_hitProxyCached = false;
		std::vector<Color> m_cachedHitProxyData;
	};
}
