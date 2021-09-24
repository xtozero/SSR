#pragma once

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

class SceneRenderer
{
public:
	virtual bool PreRender( RenderViewGroup& renderViewGroup ) = 0;
	virtual void Render( RenderViewGroup& renderViewGroup ) = 0;
	virtual void PostRender( RenderViewGroup& renderViewGroup ) = 0;

	virtual void SetRenderTarget( aga::ICommandList& commandList, RenderViewGroup& renderViewGroup ) = 0;
	void SetViewPort( aga::ICommandList& commandList, const RenderViewGroup& renderViewGroup );

	static void WaitUntilRenderingIsFinish( );

protected:
	void RenderTexturedSky( IScene& scene, RenderViewGroup& renderViewGroup );
	void RenderMesh( IScene& scene, RenderViewGroup& renderViewGroup, size_t curView );

	RenderingShaderResource m_shaderResources;
};
