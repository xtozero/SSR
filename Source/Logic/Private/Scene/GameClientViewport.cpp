#include "stdafx.h"
#include "Scene/GameClientViewport.h"

#include "Components/CameraComponent.h"
#include "Config/DefaultLogicConfig.h"
#include "GameObject/Player.h"
#include "GraphicsResource/Viewport.h"
#include "IAgl.h"
#include "Renderer/IRenderCore.h"
#include "Renderer/RenderView.h"
#include "TaskScheduler.h"
#include "World/World.h"

using ::DirectX::XMConvertToRadians;

namespace logic
{
	void GameClientViewport::Draw( World& world, rendercore::Canvas& canvas )
	{
		if ( world.Scene() == nullptr )
		{
			return;
		}

		if ( m_viewport == nullptr )
		{
			return;
		}

		auto renderModule = GetInterface<rendercore::IRenderCore>();
		if ( renderModule->IsReady() == false )
		{
			return;
		}

		const float4& bgColor = DefaultLogic::GetDefaultBackgroundColor();

		const auto& timer = world.GetTimer();
		rendercore::RenderViewGroupInitializer initializer = {
			.m_scene = *world.Scene(),
			.m_cavas = canvas,
			.m_viewport = *m_viewport,
			.m_elapsedTime = timer.GetElapsedTime(),
			.m_totalTime = timer.GetTotalTime(),
			.m_backgroundColor = { bgColor[0], bgColor[1], bgColor[2], bgColor[3] }
		};

		rendercore::RenderViewGroup renderViewGroup( initializer );
		InitView( world, renderViewGroup );
		EnqueueRenderTask(
			[this, renderModule, renderViewGroup]() mutable
			{
				if ( renderModule )
				{
					renderModule->BeginRenderingViewGroup( renderViewGroup );
				}
			} );
	}

	void GameClientViewport::SetViewport( rendercore::Viewport* viewport )
	{
		m_viewport = viewport;
	}

	rendercore::Viewport* GameClientViewport::GetViewport()
	{
		return m_viewport;
	}

	void GameClientViewport::AppSizeChanged( const std::pair<uint32, uint32>& newSize )
	{
		if ( m_viewport == nullptr )
		{
			return;
		}
		else if ( m_viewport->Size() == newSize )
		{
			return;
		}

		EnqueueRenderTask(
			[viewport = m_viewport, appSize = newSize]()
			{
				viewport->Resize( appSize );
			} );
	}

	void GameClientViewport::InitView( World& world, rendercore::RenderViewGroup& views )
	{
		CPlayer* localPlayer = GetLocalPlayer( world );
		if ( localPlayer == nullptr )
		{
			return;
		}

		const CameraComponent* cameraComponent = localPlayer->GetCameraComponent();
		if ( cameraComponent == nullptr )
		{
			return;
		}

		rendercore::RenderView& localPlayerView = views.AddRenderView();

		localPlayerView.m_viewOrigin = cameraComponent->GetPosition();
		localPlayerView.m_viewAxis = BasisVectorMatrix( cameraComponent->GetRightVector(),
			cameraComponent->GetUpVector(),
			cameraComponent->GetForwardVector() );

		localPlayerView.m_nearPlaneDistance = 1.f;
		localPlayerView.m_farPlaneDistance = 1500.f;

		auto renderTargetSize = m_viewport->Size();
		float width = static_cast<float>( renderTargetSize.first );
		float height = static_cast<float>( renderTargetSize.second );
		localPlayerView.m_aspect = width / height;
		localPlayerView.m_fov = XMConvertToRadians( 60.f );
	}
}