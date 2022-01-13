#pragma once

#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "PassProcessor.h"
#include "Scene/ShadowInfo.h"
#include "Texture.h"

#include <map>
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
	void ClearResources();

private:
	std::vector<std::string> m_parameterNames;
	std::vector<GraphicsApiResource*> m_resources;
};

struct RenderingOutputContext
{
	RefHandle<aga::Texture> m_renderTargets[MAX_RENDER_TARGET] = {};
	RefHandle<aga::Texture> m_depthStencil;

	CubeArea<float> m_viewport;
	RectangleArea<int32> m_scissorRects;
};

class SceneRenderer
{
public:
	virtual bool PreRender( RenderViewGroup& renderViewGroup );
	virtual void Render( RenderViewGroup& renderViewGroup ) = 0;
	virtual void PostRender( RenderViewGroup& renderViewGroup );

	virtual void RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView ) = 0;

	template <typename CommandList>
	void ApplyOutputContext( CommandList& commandList )
	{
		aga::Texture* renderTargets[MAX_RENDER_TARGET] = {};

		for ( uint32 i = 0; i < MAX_RENDER_TARGET; ++i )
		{
			renderTargets[i] = m_outputContext.m_renderTargets[i];
		}

		commandList.BindRenderTargets( renderTargets, MAX_RENDER_TARGET, m_outputContext.m_depthStencil );
		commandList.SetViewports( 1, &m_outputContext.m_viewport );
		commandList.SetScissorRects( 1, &m_outputContext.m_scissorRects );
	}

	static void WaitUntilRenderingIsFinish();

	virtual ~SceneRenderer() = default;
protected:
	void InitDynamicShadows( RenderViewGroup& renderViewGroup );
	void ClassifyShadowCasterAndReceiver( IScene& scene, const std::vector<ShadowInfo*>& shadows );
	void SetupShadow();
	void AllocateShadowMaps();
	void AllocateCascadeShadowMaps( const std::vector<ShadowInfo*>& shadows );

	void RenderShadowDepthPass();
	void RenderTexturedSky( IScene& scene );
	void RenderMesh( IScene& scene, RenderPass passType, RenderView& renderView );
	void RenderShadow();

	void StoreOuputContext( const RenderingOutputContext& context );

	RenderingShaderResource m_shaderResources;
	RenderingOutputContext m_outputContext;

	std::vector<ShadowInfo> m_shadowInfos;
};
