#pragma once

#include "Core/DebugConsole.h"
#include "common.h"
#include "GameObject/PickingManager.h"
#include "GameObject/Player.h"
#include "GraphicsResource/Viewport.h"
#include "ILogic.h"
//#include "Model/ModelBuilder.h"
//#include "Model/ModelManager.h"
//#include "Model/Surface.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollideBroad.h"
// #include "Render/Resource.h"
#include "Scene/DebugOverlayManager.h"
#include "Scene/SSRManager.h"
#include "SizedTypes.h"
#include "UI/ImUI.h"
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

class CGameObject;
class GameClientViewport;

class CGameLogic : public ILogic
{
public:
	virtual bool BootUp( IPlatform& platform ) override;
	virtual void Update() override;
	virtual void Pause() override;
	virtual void Resume() override;
	virtual void HandleUserInput( const UserInput& input ) override;
	virtual void AppSizeChanged( IPlatform& platform ) override;

	void SpawnObject( Owner<CGameObject*> object );

	//IRenderer& GetRenderer( ) const { return *m_pRenderer; }
	//CModelManager& GetModelManager( ) { return m_modelManager; }
	//ImUI& GetUIManager( ) { return m_ui; }
	//RE_HANDLE GetCommonConstantBuffer( uint32 purpose ) { return m_commonConstantBuffer[purpose]; }
	const std::pair<uint32, uint32>& GetAPPSize() { return m_appSize; }

	InputController* GetInputController();

private:
	void Shutdown();

	void StartLogic();
	void ProcessLogic();
	void EndLogic();

	bool LoadWorld( const char* filePath );

	void DrawScene();
	void DrawForDebug();
	void DrawDebugOverlay();
	void UpdateUIDrawInfo();
	void SceneEnd();

	void BuildRenderableList();
	void DrawReflectRenderable();

	void HandleDeviceLost();
	bool CreateDeviceDependentResource();
	bool CreateDefaultFontResource();

public:
	CGameLogic();
	virtual ~CGameLogic() override;

private:
	void CreateGameViewport();

	HMODULE m_renderCoreDll;

	HWND	m_wndHwnd;
	std::pair<uint32, uint32> m_appSize;

	std::unique_ptr<InputController> m_inputController;
	//CPickingManager m_pickingManager;
	//CSSRManager m_ssrManager;
	//CAtmosphericScatteringManager m_atmosphereManager;
	//CModelManager m_modelManager;
	rendercore::IRenderCore* m_pRenderCore = nullptr;

	//RE_HANDLE m_commonConstantBuffer[SHARED_CONSTANT_BUFFER::Count];

	// Immediate Mode UI
	//ImUI m_ui;

	//struct ImUiDrawBuffer
	//{
	//	RE_HANDLE m_buffer = RE_HANDLE::InValidHandle( );
	//	uint32 m_prevBufferSize = 0;
	//};

	//ImUiDrawBuffer m_uiDrawBuffer[2];
	//Material m_uiMaterial = INVALID_MATERIAL;
	Matrix m_uiProjMat;

	CDebugOverlayManager m_debugOverlay;

	World m_world;

	std::unique_ptr<rendercore::Viewport> m_primayViewport;
	GameClientViewport* m_gameViewport;

	std::atomic<int64> m_numDrawRequestQueued = 0;
#ifdef DEBUGGING_BY_CONSOLE
	CDebugConsole m_commandConsole;
#endif
};
