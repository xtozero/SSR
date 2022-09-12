#pragma once

#include "GameObject.h"
#include "SizedTypes.h"

namespace rendercore
{
	class Viewport;
	class RenderViewGroup;
}

class GameClientViewport : public CGameObject
{
	GENERATE_CLASS_TYPE_INFO( GameClientViewport )

public:
	void Draw( );

	void SetViewPort( rendercore::Viewport* viewport );

	void AppSizeChanged( void* handle, const std::pair<uint32, uint32>& newSize );

	explicit GameClientViewport( rendercore::Viewport* viewport ) : m_viewport( viewport ) { }

private:
	void InitView( rendercore::RenderViewGroup& views );

	rendercore::Viewport* m_viewport = nullptr;

	uint32 m_drawFence = 0;
	uint32 m_curDrawFence = 0;

	float m_clearColor[4] = { 0.f, 0.f, 0.f, 1.f };
};
