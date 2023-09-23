#include "stdafx.h"
#include "World/World.h"

#include "GameObject/Player.h"
#include "InterfaceFactories.h"
#include "Physics/PhysicsScene.h"
#include "Renderer/IRenderCore.h"
#include "Scene/DebugOverlayManager.h"
#include "Scene/IScene.h"
#include "TaskScheduler.h"

namespace logic
{
	void StartPhysicsThinkFunction::ExecuteThink( float elapsedTime )
	{
		m_target->BeginPhysicsFrame( elapsedTime );
	}

	void EndPhysicsThinkFunction::ExecuteThink( [[maybe_unused]] float elapsedTime )
	{
		m_target->EndPhysicsFrame();
	}

	void World::OnDeviceRestore( CGameLogic& gameLogic )
	{
		for ( auto& object : m_gameObjects )
		{
			object->OnDeviceRestore( gameLogic );
		}
	}

	void World::Initialize()
	{
		m_scene = GetInterface<rendercore::IRenderCore>()->CreateScene();

		CreatePhysicsScene();
	}

	void World::CleanUp()
	{
		m_gameObjects.clear();

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

		for ( auto object = m_gameObjects.begin(); object != m_gameObjects.end(); )
		{
			CGameObject* candidate = object->get();
			if ( candidate->WillRemove() )
			{
				// OnObjectRemoved( candidate->GetRigidBody( ) );
				object = m_gameObjects.erase( object );
			}
			else
			{
				++object;
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
		object->SetID( m_gameObjects.size() );
		m_gameObjects.emplace_back( object );
	}

	//void World::DebugDrawBVH( CDebugOverlayManager& debugOverlay, uint32 color, float duration )
	//{
	//	for ( auto node : m_bvhTree )
	//	{
	//		debugOverlay.AddDebugSphere( node.m_volume.GetCenter(), node.m_volume.GetRadius(), color, duration );
	//	}
	//}

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
