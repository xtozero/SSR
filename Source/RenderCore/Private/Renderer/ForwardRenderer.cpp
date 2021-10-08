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

bool ForwardRenderer::PreRender( RenderViewGroup& renderViewGroup )
{
	auto rendertargetSize = renderViewGroup.Viewport( ).Size( );

	auto& depthStencil = m_renderTargets.GetDepthStencil( );
	depthStencil.UpdateBufferSize( rendertargetSize.first, rendertargetSize.second );

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
	auto& depthStencil = m_renderTargets.GetDepthStencil( );
	depthStencil.Clear( 1.f, 0 );

	IScene& scene = renderViewGroup.Scene( );

	auto& viewConstant = scene.SceneViewConstant( );

	for ( size_t i = 0; i < renderViewGroup.Size( ); ++i )
	{
		auto& view = renderViewGroup[i];

		ViewConstantBufferParameters viewConstantParam;
		FillViewConstantParam( viewConstantParam, scene.GetRenderScene( ), view );

		viewConstant.Update( viewConstantParam );

		m_shaderResources.AddResource( "VEIW_PROJECTION", viewConstant.Resource( ) );
		m_shaderResources.AddResource( "ForwardLightConstant", view.m_forwardLighting->m_lightConstant.Resource( ) );
		m_shaderResources.AddResource( "ForwardLight", view.m_forwardLighting->m_lightBuffer.SRV( ) );

		RenderTexturedSky( scene, renderViewGroup );
		RenderMesh( scene, renderViewGroup, i );
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

void ForwardRenderer::SetRenderTarget( aga::ICommandList& commandList, RenderViewGroup& renderViewGroup )
{
	auto depthStencil = m_renderTargets.GetDepthStencil( ).Texture( );
	auto renderTarget = renderViewGroup.Viewport( ).Texture( );

	commandList.BindRenderTargets( &renderTarget, 1, depthStencil );
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
