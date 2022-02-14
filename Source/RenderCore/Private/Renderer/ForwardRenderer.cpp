#include "stdafx.h"
#include "ForwardRenderer.h"

#include "CommandList.h"
#include "ForwardLighting.h"
#include "Math/Vector.h"
#include "Proxies/LightProxy.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"
#include "SkyAtmosphereRendering.h"
#include "Viewport.h"

void ForwardRendererRenderTargets::UpdateBufferSize( uint32 width, uint32 height )
{
	std::pair<uint32, uint32> newBufferSize( width, height );
	if ( m_bufferSize != newBufferSize )
	{
		ReleaseAll();
		m_bufferSize = newBufferSize;
	}
}

aga::Texture* ForwardRendererRenderTargets::GetDepthStencil()
{
	AllocDepthStencil();
	return m_depthStencil;
}

aga::Texture* ForwardRendererRenderTargets::GetLinearDepth()
{
	AllocLinearDepth();
	return m_linearDepth;
}

aga::Texture* ForwardRendererRenderTargets::GetWorldNormal()
{
	AllocWorldNormal();
	return m_worldNormal;
}

void ForwardRendererRenderTargets::AllocDepthStencil()
{
	if ( m_depthStencil == nullptr )
	{
		TEXTURE_TRAIT trait = {
			m_bufferSize.first,
			m_bufferSize.second,
			1,
			1,
			0,
			1,
			RESOURCE_FORMAT::D24_UNORM_S8_UINT,
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			RESOURCE_BIND_TYPE::DEPTH_STENCIL,
			RESOURCE_MISC::NONE
		};

		m_depthStencil = aga::Texture::Create( trait );
		EnqueueRenderTask( [depthStencil = m_depthStencil]()
			{
				depthStencil->Init();
			} );
	}
}

void ForwardRendererRenderTargets::AllocLinearDepth()
{
	if ( m_linearDepth == nullptr )
	{
		TEXTURE_TRAIT trait = {
			m_bufferSize.first,
			m_bufferSize.second,
			1,
			1,
			0,
			1,
			RESOURCE_FORMAT::R32_FLOAT,
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			RESOURCE_BIND_TYPE::RENDER_TARGET | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
			0
		};

		m_linearDepth = aga::Texture::Create( trait );
		EnqueueRenderTask( [linearDepth = m_linearDepth]()
			{
				linearDepth->Init();
			} );
	}
}

void ForwardRendererRenderTargets::AllocWorldNormal()
{
	if ( m_worldNormal == nullptr )
	{
		TEXTURE_TRAIT trait = {
			m_bufferSize.first,
			m_bufferSize.second,
			1,
			1,
			0,
			1,
			RESOURCE_FORMAT::R10G10B10A2_UNORM,
			RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
			RESOURCE_BIND_TYPE::RENDER_TARGET | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
			0
		};

		m_worldNormal = aga::Texture::Create( trait );
		EnqueueRenderTask( [worldNormal = m_worldNormal]()
			{
				worldNormal->Init();
			} );
	}
}

void ForwardRendererRenderTargets::ReleaseAll()
{
	m_depthStencil = nullptr;
	m_linearDepth = nullptr;
	m_worldNormal = nullptr;
}

bool ForwardRenderer::PreRender( RenderViewGroup& renderViewGroup )
{
	SceneRenderer::PreRender( renderViewGroup );

	auto rendertargetSize = renderViewGroup.Viewport().Size();
	m_renderTargets.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );

	IScene& scene = renderViewGroup.Scene();
	bool prepared = UpdateGPUPrimitiveInfos( *scene.GetRenderScene() );

	auto& gpuPrimitiveInfo = scene.GpuPrimitiveInfo();
	m_shaderResources.AddResource( "primitiveInfo", gpuPrimitiveInfo.SRV() );
	m_shaderResources.AddResource( "SceneDepth", m_renderTargets.GetLinearDepth()->SRV() );
	m_shaderResources.AddResource( "WorldNormal", m_renderTargets.GetWorldNormal()->SRV() );

	SamplerOption defaultSamplerOption;
	SamplerState defaultSampler = GraphicsInterface().FindOrCreate( defaultSamplerOption );
	m_shaderResources.AddResource( "SceneDepthSampler", defaultSampler.Resource() );
	m_shaderResources.AddResource( "WorldNormalSampler", defaultSampler.Resource() );

	if ( prepared )
	{
		for ( auto& view : renderViewGroup )
		{
			view.m_forwardLighting = rendercore::Allocator().Allocate<ForwardLightingResource>( 1 );
			std::construct_at( view.m_forwardLighting );
		}

		UpdateLightResource( renderViewGroup );
	}

	return prepared;
}

void ForwardRenderer::Render( RenderViewGroup& renderViewGroup )
{
	RenderShadowDepthPass();

	IScene& scene = renderViewGroup.Scene();
	if ( Scene* renderScene = scene.GetRenderScene() )
	{
		rendercore::RenderAtmosphereLookUpTables( *renderScene );
	}

	for ( uint32 i = 0; i < static_cast<uint32>( renderViewGroup.Size() ); ++i )
	{
		auto& viewConstant = scene.SceneViewConstant();

		ViewConstantBufferParameters viewConstantParam;
		FillViewConstantParam( viewConstantParam, scene.GetRenderScene(), renderViewGroup, i );

		viewConstant.Update( viewConstantParam );

		auto& view = renderViewGroup[i];

		m_shaderResources.AddResource( "VEIW_PROJECTION", viewConstant.Resource() );
		m_shaderResources.AddResource( "ForwardLightConstant", view.m_forwardLighting->m_lightConstant.Resource() );
		m_shaderResources.AddResource( "ForwardLight", view.m_forwardLighting->m_lightBuffer.SRV() );

		RenderDepthPass( renderViewGroup, i );
		RenderDefaultPass( renderViewGroup, i );

		RenderShadow();

		RenderSkyAtmosphere( scene, view );

		RenderVolumetricCloud( scene, view );
	}
}

void ForwardRenderer::PostRender( RenderViewGroup& renderViewGroup )
{
	for ( auto& view : renderViewGroup )
	{
		std::destroy_at( view.m_forwardLighting );
		view.m_forwardLighting = nullptr;
	}

	SceneRenderer::PostRender( renderViewGroup );
}

void ForwardRenderer::RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView )
{
	auto renderTarget = renderViewGroup.Viewport().Texture();
	auto depthStencil = m_renderTargets.GetDepthStencil();

	auto [width, height] = renderViewGroup.Viewport().Size();

	RenderingOutputContext context = {
		{ renderTarget },
		depthStencil,
		{ 0.f, 0.f, static_cast<float>( width ), static_cast<float>( height ), 0.f, 1.f },
		{ 0L, 0L, static_cast<int32>( width ), static_cast<int32>( height ) }
	};
	StoreOuputContext( context );

	IScene& scene = renderViewGroup.Scene();
	RenderTexturedSky( scene );

	RenderMesh( scene, RenderPass::Default, renderViewGroup[curView] );
}

void ForwardRenderer::RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView )
{
	auto renderTarget = m_renderTargets.GetLinearDepth();
	auto worldNormal = m_renderTargets.GetWorldNormal();
	auto depthStencil = m_renderTargets.GetDepthStencil();

	auto [width, height] = renderViewGroup.Viewport().Size();

	RenderingOutputContext context = {
		{ renderTarget, worldNormal },
		depthStencil,
		{ 0.f, 0.f, static_cast<float>( width ), static_cast<float>( height ), 0.f, 1.f },
		{ 0L, 0L, static_cast<int32>( width ), static_cast<int32>( height ) }
	};
	StoreOuputContext( context );

	auto commandList = rendercore::GetImmediateCommandList();

	aga::RenderTargetView* rtv0 = renderTarget != nullptr ? renderTarget->RTV() : nullptr;
	commandList.ClearRenderTarget( rtv0, { 1, 1, 1, 1 } );

	aga::RenderTargetView* rtv1 = worldNormal != nullptr ? worldNormal->RTV() : nullptr;
	commandList.ClearRenderTarget( rtv1, { } );

	aga::DepthStencilView* dsv = depthStencil != nullptr ? depthStencil->DSV() : nullptr;
	commandList.ClearDepthStencil( dsv, 1.f, 0 );

	IScene& scene = renderViewGroup.Scene();
	RenderMesh( scene, RenderPass::DepthWrite, renderViewGroup[curView] );
}

void ForwardRenderer::UpdateLightResource( RenderViewGroup& renderViewGroup )
{
	Scene* scene = renderViewGroup.Scene().GetRenderScene();
	if ( scene == nullptr )
	{
		return;
	}

	rendercore::VectorSingleFrame<LightSceneInfo*> validLights;
	const SparseArray<LightSceneInfo*>& lights = scene->Lights();
	for ( auto light : lights )
	{
		validLights.emplace_back( light );
	}

	for ( auto& view : renderViewGroup )
	{
		ForwardLightBuffer& lightBuffer = view.m_forwardLighting->m_lightBuffer;

		uint32 numElement = static_cast<uint32>( ( sizeof( ForwardLightData ) / sizeof( Vector4 ) ) * validLights.size() );
		lightBuffer.Initialize( sizeof( Vector4 ), numElement, RESOURCE_FORMAT::R32G32B32A32_FLOAT );

		auto lightData = static_cast<ForwardLightData*>( lightBuffer.Lock() );
		if ( lightData == nullptr )
		{
			continue;
		}

		for ( auto light : validLights )
		{
			LightProxy* proxy = light->Proxy();
			LightProperty property = proxy->GetLightProperty();

			lightData->m_positionAndRange = Vector4( property.m_position[0], property.m_position[1], property.m_position[2], property.m_range );
			lightData->m_diffuse = property.m_diffuse;
			lightData->m_specular = property.m_specular;
			lightData->m_attenuationAndFalloff = Vector4( property.m_attenuation[0], property.m_attenuation[1], property.m_attenuation[2], property.m_fallOff );
			lightData->m_directionAndType = Vector4( property.m_direction[0], property.m_direction[1], property.m_direction[2], static_cast<float>( property.m_type ) );
			lightData->m_spotAngles = Vector4( property.m_theta, property.m_phi, 0.f, 0.f );

			++lightData;
		}

		lightBuffer.Unlock();

		ForwardLightConstant lightConstant = {
			static_cast<uint32>( validLights.size() ),
			view.m_viewOrigin
		};

		view.m_forwardLighting->m_lightConstant.Update( lightConstant );
	}
}
