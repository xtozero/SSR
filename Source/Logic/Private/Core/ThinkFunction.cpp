#include "Core/ThinkFunction.h"

#include "Components/Component.h"
#include "GameObject/GameObject.h"
#include "World/World.h"

bool ThinkFunction::IsThinkFunctionRegistered()
{
	return ( m_internalData.get() != nullptr ) && m_internalData->m_registered;
}

void ThinkFunction::RegisterThinkFunction( World& world )
{
	if ( IsThinkFunctionRegistered() == false )
	{
		if ( m_internalData.get() == nullptr )
		{
			m_internalData.reset( new InternalData( world ) );

			ThinkTaskManager& thinkTaskManager = world.GetThinkTaskManager();
			thinkTaskManager.RegisterThinkFunction( this );

			m_internalData->m_registered = true;
		}
	}
}

void ThinkFunction::UnRegisterThinkFunction()
{
	if ( IsThinkFunctionRegistered() )
	{
		World& world = m_internalData->m_world;

		ThinkTaskManager& thinkTaskManager = world.GetThinkTaskManager();
		thinkTaskManager.UnRegisterThinkFunction( this );

		m_internalData->m_registered = false;
	}
}

void GameObjectThinkFunction::ExecuteThink( float elapsedTime )
{
	m_target->Think( elapsedTime );
}

void ComponentThinkFunction::ExecuteThink( float elapsedTime )
{
	m_target->ThinkComponent( elapsedTime );
}
