#include "stdafx.h"
#include "GameObject/GameClientViewport.h"

#include "GraphicsResource/Viewport.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"

void GameClientViewport::Draw( )
{
	if ( m_drawFence < m_curDrawFence )
	{
		return;
	}

	++m_curDrawFence;

	ENQUEUE_THREAD_TASK<ThreadType::RenderThread>( [viewport = m_viewport, clearColor = &m_clearColor]( )
	{
		viewport->Clear( *clearColor );
	} );

	auto renderTargetSize = m_viewport->Size( );
	RECT rect = { 
		0,
		0,
		static_cast<long>( renderTargetSize.first ),
		static_cast<long>( renderTargetSize.second ) };

	ENQUEUE_THREAD_TASK<ThreadType::RenderThread>( [aga = GetInterface<IAga>( ), rect]()
	{
		if ( aga )
		{
			aga->SetViewport( rect.left, rect.top, 0.f, rect.right, rect.bottom, 1.f );
			aga->SetScissorRect( rect.left, rect.top, rect.right, rect.bottom );
		}
	} );

	ENQUEUE_THREAD_TASK<ThreadType::RenderThread>( [this, viewport = m_viewport]( )
	{
		viewport->Present( false );
		++m_drawFence;
	} );
}

void GameClientViewport::SetViewPort( rendercore::Viewport* viewport )
{
	m_viewport = viewport;
}
