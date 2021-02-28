#include "stdafx.h"
#include "Renderer/SceneRenderer.h"

#include "Proxies/PrimitiveProxy.h"
#include "RenderView.h"
#include "Scene/IScene.h"
#include "Scene/PrimitiveSceneInfo.h"

void SceneRenderer::RenderMesh( const IScene& scene, RenderView& view )
{
	const auto& primitives = scene.Primitives( );

	for ( auto primitive : primitives )
	{
		PrimitiveProxy* proxy = primitive->m_sceneProxy;
		proxy->TakeSnapshot( view.m_drawSnapshots );
	}
}
