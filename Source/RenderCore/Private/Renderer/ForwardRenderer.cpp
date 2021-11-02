#include "stdafx.h"
#include "ForwardRenderer.h"

#include "AbstractGraphicsInterface.h"
#include "ForwardLighting.h"
#include "Proxies/LightProxy.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/Scene.h"
#include "Scene/SceneConstantBuffers.h"
#include "Viewport.h"

void ForwardRendererRenderTargets::UpdateBufferSize( uint32 width, uint32 height )
{
	std::pair<uint32, uint32> newBufferSize( width, height );
	if ( m_bufferSize != newBufferSize )
	{
		ReleaseAll( );
		m_bufferSize = newBufferSize;
	}
}

aga::Texture* ForwardRendererRenderTargets::GetDepthStencil( )
{
	AllocDepthStencil( );
	return m_depthStencil;
}

aga::Texture* ForwardRendererRenderTargets::GetLinearDepth( )
{
	AllocLinearDepth( );
	return m_linearDepth.Get( );
}

void ForwardRendererRenderTargets::AllocDepthStencil( )
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
		EnqueueRenderTask( [depthStencil = m_depthStencil]( )
		{
			depthStencil->Init( );
		} );
	}
}

void ForwardRendererRenderTargets::AllocLinearDepth( )
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
		EnqueueRenderTask( [linearDepth = m_linearDepth]( )
		{
			linearDepth->Init( );
		} );
	}
}

void ForwardRendererRenderTargets::ReleaseAll( )
{
	m_depthStencil = nullptr;
	m_linearDepth = nullptr;
}

bool ForwardRenderer::PreRender( RenderViewGroup& renderViewGroup )
{
	auto rendertargetSize = renderViewGroup.Viewport( ).Size( );
	m_renderTargets.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );

	IScene& scene = renderViewGroup.Scene( );
	bool prepared = UpdateGPUPrimitiveInfos( *scene.GetRenderScene( ) );

	auto& gpuPrimitiveInfo = scene.GpuPrimitiveInfo( );
	m_shaderResources.AddResource( "primitiveInfo", gpuPrimitiveInfo.SRV( ) );

	if ( prepared )
	{
		for ( auto& view : renderViewGroup )
		{
			view.m_forwardLighting = new ForwardLightingResource( );
		}

		UpdateLightResource( renderViewGroup );
	}

	return prepared;
}

void ForwardRenderer::Render( RenderViewGroup& renderViewGroup )
{
	for ( uint32 i = 0; i < static_cast<uint32>( renderViewGroup.Size( ) ); ++i )
	{
		IScene& scene = renderViewGroup.Scene( );

		auto& viewConstant = scene.SceneViewConstant( );

		auto& view = renderViewGroup[i];

		ViewConstantBufferParameters viewConstantParam;
		FillViewConstantParam( viewConstantParam, scene.GetRenderScene( ), view );

		viewConstant.Update( viewConstantParam );

		m_shaderResources.AddResource( "VEIW_PROJECTION", viewConstant.Resource( ) );
		m_shaderResources.AddResource( "ForwardLightConstant", view.m_forwardLighting->m_lightConstant.Resource( ) );
		m_shaderResources.AddResource( "ForwardLight", view.m_forwardLighting->m_lightBuffer.SRV( ) );

		RenderDepthPass( renderViewGroup, i );
		RenderDefaultPass( renderViewGroup, i );
	}
}

void ForwardRenderer::PostRender( RenderViewGroup& renderViewGroup )
{
	m_shaderResources.ClearResources( );

	for ( auto& view : renderViewGroup )
	{
		delete view.m_forwardLighting;
		view.m_forwardLighting = nullptr;
	}
}

void ForwardRenderer::RenderDepthPass( RenderViewGroup& renderViewGroup, uint32 curView )
{
	auto renderTarget = m_renderTargets.GetLinearDepth( );
	auto depthStencil = m_renderTargets.GetDepthStencil( );

	auto[width, height] = renderViewGroup.Viewport( ).Size( );

	RenderingOutputContext context = {
		renderTarget,
		depthStencil,
		{ 0.f, 0.f, static_cast<float>( width ), static_cast<float>( height ), 0.f, 1.f},
		{ 0L, 0L, static_cast<int32>( width ), static_cast<int32>( height ) }
	};
	StoreOuputContext( context );
	
	float clearColor[4] = { };
	GraphicsInterface( ).ClearRenderTarget( renderTarget, clearColor );
	GraphicsInterface( ).ClearDepthStencil( depthStencil, 1.f, 0 );

	IScene& scene = renderViewGroup.Scene( );
	RenderMesh( scene, RenderPass::DepthWrite, renderViewGroup[curView] );
}

void ForwardRenderer::RenderDefaultPass( RenderViewGroup& renderViewGroup, uint32 curView )
{
	auto renderTarget = renderViewGroup.Viewport( ).Texture( );
	auto depthStencil = m_renderTargets.GetDepthStencil( );

	auto[width, height] = renderViewGroup.Viewport( ).Size( );

	RenderingOutputContext context = {
		renderTarget,
		depthStencil,
		{ 0.f, 0.f, static_cast<float>( width ), static_cast<float>( height ), 0.f, 1.f},
		{ 0L, 0L, static_cast<int32>( width ), static_cast<int32>( height ) }
	};
	StoreOuputContext( context );

	IScene& scene = renderViewGroup.Scene( );
	RenderTexturedSky( scene );

	RenderMesh( scene, RenderPass::Default, renderViewGroup[curView] );
}

void ForwardRenderer::UpdateLightResource( RenderViewGroup& renderViewGroup )
{
	Scene* scene = renderViewGroup.Scene( ).GetRenderScene( );
	if ( scene == nullptr )
	{
		return;
	}

	std::vector<LightSceneInfo*> validLights;
	const SparseArray<LightSceneInfo*>& lights = scene->Lights( );
	for ( auto light : lights )
	{
		validLights.emplace_back( light );
	}
 
	for ( auto& view : renderViewGroup )
	{
		ForwardLightBuffer& lightBuffer = view.m_forwardLighting->m_lightBuffer;

		uint32 numElement = static_cast<uint32>( ( sizeof( ForwardLightData ) / sizeof( CXMFLOAT4 ) ) * validLights.size( ) );
		lightBuffer.Initialize( sizeof( CXMFLOAT4 ), numElement, RESOURCE_FORMAT::R32G32B32A32_FLOAT );

		auto lightData = static_cast<ForwardLightData*>( lightBuffer.Lock( ) );
		if ( lightData == nullptr )
		{
			continue;
		}

		for ( auto light : validLights )
		{
			LightProxy* proxy = light->Proxy( );
			LightProperty property = proxy->GetLightProperty( );

			lightData->m_positionAndRange = CXMFLOAT4( property.m_position[0], property.m_position[1], property.m_position[2], property.m_range );
			lightData->m_diffuse = property.m_diffuse;
			lightData->m_specular = property.m_specular;
			lightData->m_attenuationAndFalloff = CXMFLOAT4( property.m_attenuation[0], property.m_attenuation[1], property.m_attenuation[2], property.m_fallOff );
			lightData->m_directionAndType = CXMFLOAT4( property.m_direction[0], property.m_direction[1], property.m_direction[2], static_cast<float>( property.m_type ) );
			lightData->m_spotAngles = CXMFLOAT4( property.m_theta, property.m_phi, 0.f, 0.f );

			++lightData;
		}

		lightBuffer.Unlock( );

		ForwardLightConstant lightConstant = {
			static_cast<uint32>( validLights.size( ) ),
			view.m_viewOrigin
		};

		view.m_forwardLighting->m_lightConstant.Update( lightConstant );
	}
}
