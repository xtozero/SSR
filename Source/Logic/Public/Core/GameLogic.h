#pragma once

#include "Core/DebugConsole.h"
#include "common.h"
#include "GameObject/Player.h"
#include "GraphicsResource/Canvas.h"
#include "GraphicsResource/Viewport.h"
#include "ILogic.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollideBroad.h"
#include "Scene/SSRManager.h"
#include "SizedTypes.h"
#include "UserInput/Controller.h"
#include "World/World.h"

#include <list>
#include <memory>
#include <minwindef.h>
#include <vector>

namespace rendercore
{
	class IRenderCore;
}

namespace logic
{
	class CGameObject;
	class GameClientViewport;

	class CGameLogic : public ILogic
	{
	public:
		virtual bool BootUp( engine::IPlatform& platform ) override;
		virtual void Update() override;
		virtual void Pause() override;
		virtual void Resume() override;
		virtual void HandleUserInput( const engine::UserInput& input ) override;
		virtual void HandleTextInput( uint64 text, bool bUnicode ) override;
		virtual void AppSizeChanged( engine::IPlatform& platform ) override;
		virtual GameClientViewport* GetGameClientViewport() override;
		virtual bool LoadWorld( const char* filePath ) override;
		virtual void UnloadWorld() override;
		virtual World& GetWorld() override;

		void SpawnObject( Owner<CGameObject*> object );

		const std::pair<uint32, uint32>& GetAPPSize() { return m_appSize; }

		InputController* GetInputController();

	private:
		void Shutdown();

		void StartLogic();
		void ProcessLogic();
		void EndLogic();

		void DrawScene();
		void UpdateUIDrawInfo();
		void SceneEnd();

	public:
		CGameLogic() = default;
		virtual ~CGameLogic() override;

	private:
		void CreateGameViewport();

		HMODULE m_renderCoreDll = nullptr;

		HWND	m_wndHwnd = nullptr;
		std::pair<uint32, uint32> m_appSize;

		std::unique_ptr<InputController> m_inputController;
		rendercore::IRenderCore* m_pRenderCore = nullptr;

		World m_world;

		std::unique_ptr<rendercore::Canvas> m_canvas;
		std::unique_ptr<rendercore::Viewport> m_primayViewport;
		std::unique_ptr<GameClientViewport> m_gameViewport;

		std::atomic<int64> m_numDrawRequestQueued = 0;
#ifdef DEBUGGING_BY_CONSOLE
		CDebugConsole m_commandConsole;
#endif
	};
}
