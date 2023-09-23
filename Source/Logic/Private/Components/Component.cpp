#include "stdafx.h"
#include "Components/Component.h"

#include "GameObject/GameObject.h"
#include "TaskScheduler.h"
#include "World/World.h"

#include <cassert>

namespace logic
{
	void Component::RegisterComponent()
	{
		RegisterComponent( m_pOwner->GetWorld() );
	}

	void Component::UnregisterComponent()
	{
		DestroyPhysicsState();

		if ( m_renderStateCreated )
		{
			RemoveRenderState();
		}
	}

	void Component::RecreateRenderState()
	{
		if ( m_renderStateCreated )
		{
			RemoveRenderState();
		}

		CreateRenderState();
	}

	void Component::SendRenderTransform()
	{
		m_markForSendRenderTransform = false;
		m_renderTransformDirty = false;
	}

	void Component::UpdateState()
	{
		m_markForUpdateState = false;

		if ( m_renderStateDirty )
		{
			m_renderStateDirty = false;
			RecreateRenderState();
		}
	}

	void Component::MarkRenderStateDirty()
	{
		if ( m_renderStateCreated && ( m_renderStateDirty == false ) )
		{
			m_renderStateDirty = true;

			if ( m_markForUpdateState == false )
			{
				EnqueueThreadTask<ThreadType::GameThread>(
					[this]()
					{
						UpdateState();
					} );
			}

			m_markForUpdateState = true;
		}
	}

	void Component::MarkRenderTransformDirty()
	{
		if ( m_renderStateCreated && ( m_renderTransformDirty == false ) )
		{
			m_renderTransformDirty = true;

			if ( m_markForSendRenderTransform == false )
			{
				EnqueueThreadTask<ThreadType::GameThread>(
					[this]()
					{
						SendRenderTransform();
					} );
			}

			m_markForSendRenderTransform = true;
		}
	}

	void Component::RegisterThinkFunction()
	{
		if ( m_think.m_canEverTick )
		{
			assert( m_pWorld != nullptr );

			m_think.m_target = this;
			m_think.RegisterThinkFunction( *m_pWorld );
		}
	}

	void Component::UnRegisterThinkFunction()
	{
		m_think.UnRegisterThinkFunction();
	}

	CGameObject* Component::GetOwner() const
	{
		return m_pOwner;
	}

	void Component::DestroyComponent()
	{
		UnRegisterThinkFunction();
		UnregisterComponent();

		if ( CGameObject* owner = GetOwner() )
		{
			owner->RemoveComponent( this );
		}
	}

	Component::Component( CGameObject* pOwner, const char* name ) : m_pOwner( pOwner )
	{
		assert( pOwner != nullptr && "Owner can not be nullptr" );

		pOwner->m_components.emplace_back( this );
		auto result = pOwner->m_componentMap.emplace( Name( name ), this );
		assert( result.second && "Component's name duplicated" );

		m_think.m_thinkGroup = ThinkingGroup::DuringPhysics;
		m_think.m_canEverTick = false;
	}

	bool Component::ShouldCreateRenderState() const
	{
		return false;
	}

	void Component::CreateRenderState()
	{
		m_renderStateCreated = true;
	}

	void Component::RemoveRenderState()
	{
		m_renderStateCreated = false;
	}

	bool Component::PhysicsStateCreated() const
	{
		return m_physicsStateCreated;
	}

	void Component::CreatePhysicsState()
	{
		if ( m_physicsStateCreated == false && m_pWorld->GetPhysicsScene() && ShouldCreatePhysicsState() )
		{
			OnCreatePhysicsState();
		}
	}

	void Component::DestroyPhysicsState()
	{
		if ( m_physicsStateCreated )
		{
			OnDestroyPhysicsState();
		}
	}

	bool Component::ShouldCreatePhysicsState() const
	{
		return false;
	}

	void Component::OnCreatePhysicsState()
	{
		assert( m_physicsStateCreated == false );
		m_physicsStateCreated = true;
	}

	void Component::OnDestroyPhysicsState()
	{
		assert( m_physicsStateCreated );
		m_physicsStateCreated = false;
	}

	void Component::RegisterComponent( World* pWorld )
	{
		m_pWorld = pWorld;

		if ( m_renderStateCreated == false && ShouldCreateRenderState() )
		{
			CreateRenderState();
		}

		CreatePhysicsState();
	}
}
