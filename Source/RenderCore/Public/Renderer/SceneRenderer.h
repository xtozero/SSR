#pragma once

#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "PassProcessor.h"
#include "Texture.h"

#include <vector>
#include <string>

namespace aga
{
	class ShaderBindings;
	class ICommandList;
}

class GraphicsApiResource;
class IScene;
class RenderViewGroup;

struct RenderView;
struct ShaderStates;

class RenderingShaderResource
{
public:
	void BindResources( const ShaderStates& shaders, aga::ShaderBindings& bindings );
	void AddResource( const std::string& parameterName, GraphicsApiResource* resource );
	void ClearResources( );

private:
	std::vector<std::string> m_parameterNames;
	std::vector<GraphicsApiResource*> m_resources;
};

struct RenderingOutputContext
{
	RefHandle<aga::Texture> m_renderTargets;
	RefHandle<aga::Texture> m_depthStencil;

	CubeArea<float> m_viewport;
	RectangleArea<int32> m_scissorRects;
};

class SceneRenderer
{
public:
	virtual bool PreRender( RenderViewGroup& renderViewGroup ) = 0;
	virtual void Render( RenderViewGroup& renderViewGroup ) = 0;
	virtual void PostRender( RenderViewGroup& renderViewGroup ) = 0;

	virtual void RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView ) = 0;

	virtual void RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView ) = 0;

	void ApplyOutputContext( aga::ICommandList& commandList );

	static void WaitUntilRenderingIsFinish( );

protected:
	void RenderTexturedSky( IScene& scene );
	void RenderMesh( IScene& scene, RenderPass passType, RenderView& renderView );

	void StoreOuputContext( const RenderingOutputContext& context );

	RenderingShaderResource m_shaderResources;
	RenderingOutputContext m_outputContext;
};
