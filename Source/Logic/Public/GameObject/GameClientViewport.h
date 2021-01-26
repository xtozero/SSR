#pragma once
#include "GameObject.h"

namespace rendercore
{
	class Viewport;
}

class GameClientViewport : public CGameObject
{
public:
	void Draw( );

	void SetViewPort( rendercore::Viewport* viewport );

	explicit GameClientViewport( rendercore::Viewport* viewport ) : m_viewport( viewport ) { }

private:
	rendercore::Viewport* m_viewport;

	int m_drawFence = 0;
	int m_curDrawFence = 0;

	float m_clearColor[4] = { 0.f, 0.f, 1.f, 1.f };
};