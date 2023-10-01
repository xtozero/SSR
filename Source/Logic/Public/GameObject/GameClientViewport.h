#pragma once

#include "GameObject.h"
#include "SizedTypes.h"

namespace rendercore
{
	class Canvas;
	class RenderViewGroup;
	class Viewport;
}

namespace logic
{
	class GameClientViewport : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( GameClientViewport )

	public:
		void Draw( rendercore::Canvas& canvas );

		void SetViewport( rendercore::Viewport* viewport );
		LOGIC_DLL rendercore::Viewport* GetViewport();

		void AppSizeChanged( const std::pair<uint32, uint32>& newSize );

		explicit GameClientViewport( rendercore::Viewport* viewport ) : m_viewport( viewport ) { }

	private:
		void InitView( rendercore::RenderViewGroup& views );

		rendercore::Viewport* m_viewport = nullptr;
	};
}
