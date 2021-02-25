#pragma once

class RenderViewGroup;

class SceneRenderer
{
public:
	virtual bool PrepareRender( RenderViewGroup& renderViewGroup ) = 0;
	virtual void Render( RenderViewGroup& renderViewGroup ) = 0;
};