#include "stdafx.h"
#include "GameObject/GameClientViewport.h"

#include "Components/CameraComponent.h"
#include "GameObject/Player.h"
#include "GraphicsResource/Viewport.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "Renderer/IRenderCore.h"
#include "Renderer/RenderView.h"
#include "World/World.h"

void GameClientViewport::Draw( )
{
	if ( GetWorld( ) == nullptr )
	{
		return;
	}

	if ( GetWorld( )->Scene( ) == nullptr )
	{
		return;
	}

	if ( m_viewport == nullptr )
	{
		return;
	}

	if ( m_drawFence < m_curDrawFence )
	{
		return;
	}

	++m_curDrawFence;

	float clearColor[4] = { m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3] };
	EnqueueRenderTask( [viewport = m_viewport, clearColor]( )
	{
		viewport->Clear( clearColor );
	} );

	auto renderTargetSize = m_viewport->Size( );
	RECT rect = { 
		0,
		0,
		static_cast<long>( renderTargetSize.first ),
		static_cast<long>( renderTargetSize.second ) };

	EnqueueRenderTask( [aga = GetInterface<IAga>( ), rect]( )
	{
		if ( aga )
		{
			aga->SetViewport( rect.left, rect.top, 0.f, rect.right, rect.bottom, 1.f );
			aga->SetScissorRect( rect.left, rect.top, rect.right, rect.bottom );
		}
	} );

	RenderViewGroup renderViewGroup( *GetWorld()->Scene(), *m_viewport );
	InitView( renderViewGroup );
	EnqueueRenderTask( [renderModule = GetInterface<IRenderCore>( ), renderViewGroup]( ) mutable
	{
		if ( renderModule )
		{
			renderModule->BeginRenderingViewGroup( renderViewGroup );
		}
	} );

	EnqueueRenderTask( [this, viewport = m_viewport]( )
	{
		viewport->Present( false );
		++m_drawFence;
	} );
}

void GameClientViewport::SetViewPort( rendercore::Viewport* viewport )
{
	m_viewport = viewport;
}

void GameClientViewport::AppSizeChanged( void* handle, const std::pair<UINT, UINT>& newSize )
{
	if ( m_viewport == nullptr )
	{
		return;
	}
	else if ( m_viewport->Handle( ) != handle )
	{
		return;
	}
	else if ( m_viewport->Size( ) == newSize )
	{
		return;
	}

	EnqueueRenderTask( [viewport = m_viewport, appSize = newSize]( )
	{
		viewport->Resize( appSize );
	} );
}

void GameClientViewport::InitView( RenderViewGroup& views )
{
	using namespace DirectX;

	auto pWorld = GetWorld( );
	if ( pWorld == nullptr )
	{
		return;
	}

	CPlayer* localPlayer = GetLocalPlayer( *pWorld );
	if ( localPlayer == nullptr )
	{
		return;
	}

	const CameraComponent* cameraComponent = localPlayer->GetCameraComponent( );
	if ( cameraComponent == nullptr )
	{
		return;
	}

	RenderView& localPlayerView = views.AddRenderView( );

	localPlayerView.m_viewOrigin = cameraComponent->GetPosition( );
	localPlayerView.m_viewAxis = CXMFLOAT3X3( cameraComponent->GetRightVector( ),
		cameraComponent->GetUpVector( ),
		cameraComponent->GetForwardVector( ) );

	localPlayerView.m_nearPlaneDistance = 1.f;
	localPlayerView.m_farPlaneDistance = 1500.f;

	auto renderTargetSize = m_viewport->Size( );
	float width = static_cast<float>( renderTargetSize.first );
	float height = static_cast<float>( renderTargetSize.second );
	localPlayerView.m_aspect = width / height;
	localPlayerView.m_fov = XMConvertToRadians( 60.f );
}
