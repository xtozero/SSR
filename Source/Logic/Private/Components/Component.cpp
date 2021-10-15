#include "stdafx.h"
#include "Components/Component.h"

#include "GameObject/GameObject.h"
#include "TaskScheduler.h"

void Component::RegisterComponent( )
{
	RegisterComponent( m_pOwner->GetWorld( ) );
}

void Component::UnregisterComponent( )
{
	if ( m_renderStateCreated )
	{
		RemoveRenderState( );
	}
}

void Component::RecreateRenderState( )
{
	if ( m_renderStateCreated )
	{
		RemoveRenderState( );
	}

	CreateRenderState( );
}

void Component::UpdateState( )
{
	m_markForUpdateState = false;

	if ( m_renderStateDirty )
	{
		m_renderStateDirty = false;
		RecreateRenderState( );
	}
}

void Component::MarkRenderStateDirty( )
{
	if ( m_renderStateCreated && ( m_renderStateDirty == false ) )
	{
		m_renderStateDirty = true;

		if ( m_markForUpdateState == false )
		{
			EnqueueThreadTask<ThreadType::GameThread>( [this]( )
			{
				UpdateState( );
			} );
		}

		m_markForUpdateState = true;
	}
}

Component::Component( CGameObject* pOwner ) : m_pOwner( pOwner )
{
	pOwner->m_components.emplace_back( this );
}

bool Component::ShouldCreateRenderState( ) const
{
	return false;
}

void Component::CreateRenderState( )
{
	m_renderStateCreated = true;
}

void Component::RemoveRenderState( )
{
	m_renderStateCreated = false;
}

void Component::RegisterComponent( World* pWorld )
{
	m_pWorld = pWorld;

	if ( m_renderStateCreated == false && ShouldCreateRenderState( ) )
	{
		CreateRenderState( );
	}
}
