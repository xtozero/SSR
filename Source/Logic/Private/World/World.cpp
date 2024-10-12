#include "World/World.h"

#include "Components/Component.h"
#include "GameObject/DebugOverlay.h"
#include "GameObject/Player.h"
#include "InterfaceFactories.h"
#include "Physics/PhysicsScene.h"
#include "Renderer/IRenderCore.h"
#include "Scene/IScene.h"
#include "TaskScheduler.h"

namespace logic
{
	struct UpdateRenderStateHelper
	{
	public:
		static void Set( Component& component, int32 arrayIndex )
		{
			component.m_markForUpdateRenderStateArrayIndex = arrayIndex;
		}

		static int32 GetArrayIndex( Component& component )
		{
			return component.m_markForUpdateRenderStateArrayIndex;
		}

		static bool IsUpdateRenderStateReserved( Component& component )
		{
			return GetArrayIndex( component ) >= 0;
		}
	};

	void StartPhysicsThinkFunction::ExecuteThink( float elapsedTime )
	{
		m_target->BeginPhysicsFrame( elapsedTime );
	}

	void EndPhysicsThinkFunction::ExecuteThink( [[maybe_unused]] float elapsedTime )
	{
		m_target->EndPhysicsFrame();
	}

	void World::Initialize( CGameLogic& gameLogic )
	{
		m_scene = GetInterface<rendercore::IRenderCore>()->CreateScene( *this );

		constexpr int32 DefaultRenderStateUpdateArraySize = 1024;
		m_componentsThatNeedRenderStateUpdate.reserve( DefaultRenderStateUpdateArraySize );

		CreatePhysicsScene();

		m_debugOverlay = std::make_unique<DebugOverlay>();
		m_debugOverlay->Initialize( gameLogic, *this );
	}

	void World::CleanUp()
	{
		m_debugOverlay = nullptr;

		m_gameObjects.Clear();

		ReleasePhysicsScene();

		GetInterface<rendercore::IRenderCore>()->RemoveScene( m_scene );
	}

	void World::Pause()
	{
		m_clock.Pause();
	}

	void World::Resume()
	{
		m_clock.Resume();
	}

	void World::BeginFrame()
	{
		m_clock.Tick();

		for ( auto& object : m_gameObjects )
		{
			if ( object->WillRemove() )
			{
				m_gameObjects.Remove( object );
			}
		}

		float totalTime = GetTimer().GetTotalTime();
		m_thinkTaskManager.BeginFrame( totalTime );
	}

	void World::RunFrame()
	{
		SetupPhysicsThinkFunctions();

		RunThinkGroup( ThinkingGroup::PrePhysics );
		RunThinkGroup( ThinkingGroup::StartPhysics );
		RunThinkGroup( ThinkingGroup::DuringPhysics );
		RunThinkGroup( ThinkingGroup::EndPhysics );
	}

	void World::EndFrame()
	{
		RunThinkGroup( ThinkingGroup::PostPhysics );
	}

	void World::BeginPhysicsFrame( float elapsedTime )
	{
		PhysicsScene* physicsScene = GetPhysicsScene();
		if ( physicsScene )
		{
			m_hRunPhysics = EnqueueThreadTask<WorkerThreads>(
				[physicsScene, elapsedTime]()
				{
					physicsScene->BeginFrame( elapsedTime );
				} );
		}
	}

	void World::EndPhysicsFrame()
	{
		GetInterface<ITaskScheduler>()->Wait( m_hRunPhysics );

		PhysicsScene* physicsScene = GetPhysicsScene();
		if ( physicsScene )
		{
			physicsScene->EndFrame();
		}
	}

	void World::SpawnObject( CGameLogic& gameLogic, Owner<CGameObject*> object )
	{
		object->Initialize( gameLogic, *this );
		size_t idx = m_gameObjects.Add( object );
		object->SetID( idx );
	}

	void World::MarkComponentForNeededRenderStateUpdate( Component& component )
	{
		if ( UpdateRenderStateHelper::IsUpdateRenderStateReserved( component ) )
		{
			return;
		}

		auto arrayIndex = static_cast<int32>( m_componentsThatNeedRenderStateUpdate.size() );
		UpdateRenderStateHelper::Set( component, arrayIndex );

		m_componentsThatNeedRenderStateUpdate.emplace_back( &component );
	}

	void World::ClearComponentForNeededRenderStateUpdate( Component& component )
	{
		if ( UpdateRenderStateHelper::IsUpdateRenderStateReserved( component ) == false )
		{
			return;
		}

		int32 arrayIndex = UpdateRenderStateHelper::GetArrayIndex( component );
		UpdateRenderStateHelper::Set( component, -1 );

		m_componentsThatNeedRenderStateUpdate[arrayIndex] = nullptr;
	}

	void World::SendRenderStateUpdate()
	{
		for ( Component* component : m_componentsThatNeedRenderStateUpdate )
		{
			if ( component == nullptr )
			{
				continue;
			}

			component->UpdateRenderState();
			UpdateRenderStateHelper::Set( *component, -1 );
		}

		m_componentsThatNeedRenderStateUpdate.clear();
	}

	ThinkTaskManager& World::GetThinkTaskManager()
	{
		return m_thinkTaskManager;
	}

	void World::RunThinkGroup( ThinkingGroup group )
	{
		float elapsedTime = GetTimer().GetElapsedTime();
		m_thinkTaskManager.RunThinkGroup( group, elapsedTime );
	}

	void World::CreatePhysicsScene()
	{
		PhysicsScene* newScene = new PhysicsScene();
		SetPhysicsScene( newScene );
	}

	void World::ReleasePhysicsScene()
	{
		if ( m_physicsScene != nullptr )
		{
			delete m_physicsScene;
			m_physicsScene = nullptr;
		}
	}

	void World::SetPhysicsScene( PhysicsScene* scene )
	{
		if ( m_physicsScene != nullptr )
		{
			delete m_physicsScene;
		}

		m_physicsScene = scene;
	}

	void World::SetupPhysicsThinkFunctions()
	{
		m_startPhysicsThinkFunction.m_canEverTick = true;
		m_startPhysicsThinkFunction.m_target = this;

		m_endPhysicsThinkFunction.m_canEverTick = true;
		m_endPhysicsThinkFunction.m_target = this;

		bool needToRegistThinkFunction = ( m_startPhysicsThinkFunction.IsThinkFunctionRegistered() == false )
			|| ( m_endPhysicsThinkFunction.IsThinkFunctionRegistered() == false );

		if ( needToRegistThinkFunction )
		{
			m_startPhysicsThinkFunction.m_thinkGroup = ThinkingGroup::StartPhysics;
			m_startPhysicsThinkFunction.RegisterThinkFunction( *this );

			m_endPhysicsThinkFunction.m_thinkGroup = ThinkingGroup::EndPhysics;
			m_endPhysicsThinkFunction.RegisterThinkFunction( *this );
		}
	}

	CPlayer* GetLocalPlayer( World& w )
	{
		CPlayer* localPlayer = nullptr;

		const auto& gameObjects = w.GameObjects();
		for ( const auto& gameObject : gameObjects )
		{
			if ( CPlayer* player = Cast<CPlayer>( gameObject.get() ) )
			{
				localPlayer = player;
				break;
			}
		}

		return localPlayer;
	}
}
