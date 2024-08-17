#include "Scene/GameClientViewport.h"

#include "Components/CameraComponent.h"
#include "Config/DefaultLogicConfig.h"
#include "GameObject/Player.h"
#include "GraphicsResource/Viewport.h"
#include "IAgl.h"
#include "Proxies/HitProxy.h"
#include "Renderer/IRenderCore.h"
#include "Renderer/RenderView.h"
#include "Scene/IScene.h"
#include "TaskScheduler.h"
#include "World/World.h"

using ::DirectX::XMConvertToRadians;

namespace logic
{
	void GameClientViewport::BeginFrameRendering( rendercore::Canvas& canvas )
	{
		auto renderModule = GetInterface<rendercore::IRenderCore>();
		if ( renderModule == nullptr )
		{
			return;
		}

		EnqueueRenderTask(
			[renderModule, canvas]() mutable
			{
				if ( renderModule )
				{
					renderModule->BeginFrameRendering( canvas );
				}
			} );
	}

	void GameClientViewport::Draw( rendercore::Canvas& canvas, const rendercore::RenderViewShowFlags* showFlags )
	{
		auto renderModule = GetInterface<rendercore::IRenderCore>();
		if ( renderModule == nullptr )
		{
			return;
		}

		World& world = GetWorld();
		world.SendRenderStateUpdate();

		const auto& timer = world.GetTimer();
		rendercore::RenderViewGroupInitializer initializer = {
			.m_scene = *world.Scene(),
			.m_cavas = canvas,
			.m_viewport = *GetViewport(),
			.m_elapsedTime = timer.GetElapsedTime(),
			.m_totalTime = timer.GetTotalTime(),
			.m_showFlags = showFlags
		};

		rendercore::RenderViewGroup renderViewGroup( initializer );

		InitView( renderViewGroup );

		EnqueueRenderTask(
			[renderModule, renderViewGroup]() mutable
			{
				if ( renderModule )
				{
					renderModule->BeginRenderingViewGroup( renderViewGroup );
				}
			} );
	}

	void GameClientViewport::EndFrameRendering( rendercore::Canvas& canvas )
	{
		auto renderModule = GetInterface<rendercore::IRenderCore>();
		if ( renderModule == nullptr )
		{
			return;
		}

		EnqueueRenderTask(
			[renderModule, canvas]() mutable
			{
				if ( renderModule )
				{
					renderModule->EndFrameRendering( canvas );
				}
			} );
	}

	World& GameClientViewport::GetWorld()
	{
		return m_world;
	}

	void GameClientViewport::SetViewport( rendercore::Viewport* viewport )
	{
		m_viewport = viewport;
	}

	rendercore::Viewport* GameClientViewport::GetViewport()
	{
		return m_viewport;
	}

	rendercore::HitProxy* GameClientViewport::GetHitProxy( uint32 x, uint32 y )
	{
		if ( GetViewport() == nullptr )
		{
			return nullptr;
		}

		const std::vector<Color>& cachedData = GetRawHitProxyData();

		const auto& [width, height] = GetViewport()->Size();
		uint32 index = width * y + x;

		if ( index < cachedData.size() )
		{
			rendercore::HitProxyId hitProxyId( cachedData[index] );
			return GetHitProxyById( hitProxyId );
		}

		return nullptr;
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

		InvalidateHitProxy();

		EnqueueRenderTask(
			[viewport = m_viewport, appSize = newSize]()
			{
				viewport->Resize( appSize );
			} );
	}

	LOGIC_DLL void GameClientViewport::InvalidateHitProxy()
	{
		m_hitProxyCached = false;
	}

	void GameClientViewport::InitView( rendercore::RenderViewGroup& views )
	{
		CPlayer* localPlayer = GetLocalPlayer( GetWorld() );
		if ( localPlayer == nullptr )
		{
			return;
		}

		if ( rendercore::RenderView* localPlayerView = localPlayer->CalcSceneView( views ) )
		{
			localPlayerView->m_nearPlaneDistance = 1.f;
			localPlayerView->m_farPlaneDistance = 1500.f;

			auto renderTargetSize = m_viewport->Size();
			float width = static_cast<float>( renderTargetSize.first );
			float height = static_cast<float>( renderTargetSize.second );
			localPlayerView->m_aspect = width / height;
			localPlayerView->m_fov = XMConvertToRadians( 60.f );
		}
	}

	const std::vector<Color>& GameClientViewport::GetRawHitProxyData()
	{
		if ( m_hitProxyCached )
		{
			return m_cachedHitProxyData;
		}

		if ( GetViewport() == nullptr )
		{
			return m_cachedHitProxyData;
		}

		auto renderModule = GetInterface<rendercore::IRenderCore>();
		if ( renderModule == nullptr )
		{
			return m_cachedHitProxyData;
		}

		World& world = GetWorld();
		rendercore::IScene* scene = world.Scene();
		if ( scene == nullptr )
		{
			return m_cachedHitProxyData;
		}

		rendercore::Canvas emptyCanvas;
		rendercore::RenderViewShowFlags showFlags;
		showFlags.m_bHitProxy = true;

		Draw( emptyCanvas, &showFlags );

		const auto& [width, height] = GetViewport()->Size();

		m_cachedHitProxyData.resize( width * height );
		renderModule->GetRawHitProxyData( *GetViewport(), m_cachedHitProxyData);

		m_hitProxyCached = true;
		return m_cachedHitProxyData;
	}
}
