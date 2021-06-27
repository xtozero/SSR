#pragma once

class IScene;
class RenderViewGroup;

struct RenderView;

class SceneRenderer
{
public:
	virtual bool PrepareRender( RenderViewGroup& renderViewGroup ) = 0;
	virtual void Render( RenderViewGroup& renderViewGroup ) = 0;

	static void WaitUntilRenderingIsFinish( );
protected:
	void RenderTexturedSky( IScene& scene );
	void RenderMesh( IScene& scene, RenderView& view );
};