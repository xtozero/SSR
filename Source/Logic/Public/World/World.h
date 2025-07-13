#pragma once

#include "Core/ThinkTaskManager.h"
#include "Core/Timer.h"
#include "GameObject/GameObject.h"
#include "Math/Vector.h"
#include "SizedTypes.h"
#include "SparseArray.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace rendercore
{
	class IScene;
}

namespace logic
{
	class Player;
	class PhysicsScene;

	class StartPhysicsThinkFunction : public ThinkFunction
	{
	public:
		virtual void ExecuteThink( float elapsedTime ) override;

		World* m_target = nullptr;
	};

	class EndPhysicsThinkFunction : public ThinkFunction
	{
	public:
		virtual void ExecuteThink( float elapsedTime ) override;

		World* m_target = nullptr;
	};

	class World
	{
	public:
		void Initialize( GameLogic& gameLogic );
		void CleanUp();

		const Timer& GetTimer() const { return m_clock; }
		void Pause();
		void Resume();

		void BeginFrame();
		void RunFrame();
		void EndFrame();

		void BeginPhysicsFrame( float elapsedTime );
		void EndPhysicsFrame();

		void SpawnObject( GameLogic& gameLogic, Owner<GameObject*> object );

		void MarkComponentForNeededRenderStateUpdate( Component& component );
		void ClearComponentForNeededRenderStateUpdate( Component& component );
		void SendRenderStateUpdate();

		ThinkTaskManager& GetThinkTaskManager();

		const SparseArray<std::unique_ptr<GameObject>>& GameObjects() const
		{
			return m_gameObjects;
		}

		SparseArray<std::unique_ptr<GameObject>>& GameObjects()
		{
			return m_gameObjects;
		}

		rendercore::IScene* Scene() const
		{
			return m_scene;
		}

		PhysicsScene* GetPhysicsScene() const
		{
			return m_physicsScene;
		}

		GameObject* GetDebugOverlay() const
		{
			return m_debugOverlay.get();
		}

	private:
		void RunThinkGroup( ThinkingGroup group );

		void CreatePhysicsScene();
		void ReleasePhysicsScene();
		void SetPhysicsScene( PhysicsScene* scene );
		void SetupPhysicsThinkFunctions();

		SparseArray<std::unique_ptr<GameObject>> m_gameObjects;

		StartPhysicsThinkFunction m_startPhysicsThinkFunction;
		EndPhysicsThinkFunction m_endPhysicsThinkFunction;
		TaskHandle m_hRunPhysics;

		rendercore::IScene* m_scene = nullptr;
		PhysicsScene* m_physicsScene = nullptr;

		Timer m_clock;

		ThinkTaskManager m_thinkTaskManager;

		std::vector<Component*> m_componentsThatNeedRenderStateUpdate;

		std::unique_ptr<GameObject> m_debugOverlay;
	};

	Player* GetLocalPlayer( World& w );
}