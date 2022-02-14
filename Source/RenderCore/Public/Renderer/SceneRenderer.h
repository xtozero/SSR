#pragma once

#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "PassProcessor.h"
#include "RenderCoreAllocator.h"
#include "Scene/ShadowInfo.h"
#include "Texture.h"

#include <array>
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
		aga::RenderTargetView* renderTargets[MAX_RENDER_TARGET] = {};
		aga::DepthStencilView* depthStencil = nullptr;

		for ( uint32 i = 0; i < MAX_RENDER_TARGET; ++i )
		{
			if ( m_outputContext.m_renderTargets[i] )
			{
				renderTargets[i] = m_outputContext.m_renderTargets[i]->RTV();
			}
		}

		if ( m_outputContext.m_depthStencil )
		{
			depthStencil = m_outputContext.m_depthStencil->DSV();
		}

		commandList.BindRenderTargets( renderTargets, MAX_RENDER_TARGET, depthStencil );
		commandList.SetViewports( 1, &m_outputContext.m_viewport );
		commandList.SetScissorRects( 1, &m_outputContext.m_scissorRects );
	}

	static void WaitUntilRenderingIsFinish();

	virtual ~SceneRenderer() = default;
protected:
	void InitDynamicShadows( RenderViewGroup& renderViewGroup );
	void ClassifyShadowCasterAndReceiver( IScene& scene, const rendercore::VectorSingleFrame<ShadowInfo*>& shadows );
	void SetupShadow();
	void AllocateShadowMaps();
	void AllocateCascadeShadowMaps( const rendercore::VectorSingleFrame<ShadowInfo*>& shadows );

	void RenderShadowDepthPass();
	void RenderTexturedSky( IScene& scene );
	void RenderMesh( IScene& scene, RenderPass passType, RenderView& renderView );
	void RenderShadow();
	void RenderSkyAtmosphere( IScene& scene, RenderView& renderView );
	void RenderVolumetricCloud( IScene& scene, RenderView& renderView );

	void StoreOuputContext( const RenderingOutputContext& context );

	RenderingShaderResource m_shaderResources;
	RenderingOutputContext m_outputContext;

	rendercore::VectorSingleFrame<ShadowInfo> m_shadowInfos;
	using PassVisibleSnapshots = std::array<rendercore::VectorSingleFrame<VisibleDrawSnapshot>, static_cast<uint32>( RenderPass::Count )>;
	rendercore::VectorSingleFrame<PassVisibleSnapshots> m_passSnapshots;
};
