#pragma once

class IScene;
class RenderViewGroup;

struct RenderView;

class SceneRenderer
{
public:
	virtual bool PrepareRender( RenderViewGroup& renderViewGroup ) = 0;
	virtual void Render( RenderViewGroup& renderViewGroup ) = 0;

protected:
	void RenderMesh( const IScene& scene, RenderView& view );
};