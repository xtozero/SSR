#pragma once

#include "Core/DebugConsole.h"
#include "common.h"
#include "GameObject/LightManager.h"
#include "GameObject/PickingManager.h"
#include "GameObject/Player.h"
#include "ILogic.h"
#include "Model/ModelBuilder.h"
#include "Model/ModelManager.h"
#include "Model/Surface.h"
#include "Physics/BoundingSphere.h"
#include "Physics/CollideBroad.h"
// #include "Render/Resource.h"
#include "Scene/AtmosphereScattering.h"
#include "Scene/ConstantBufferDefine.h"
#include "Scene/DebugOverlayManager.h"
#include "Scene/RenderView.h"
#include "Scene/ShadowManager.h"
#include "Scene/SSRManager.h"
#include "Timer.h"
#include "UI/ImUI.h"
#include "UserInput/MouseController.h"
#include "World/World.h"

#include <list>
#include <memory>
#include <minwindef.h>
#include <vector>

class CGameObject;
class IRenderCore;

enum RENDERABLE_TYPE
{
	OPAQUE_RENDERABLE = 0,
	TRANSPARENT_RENDERABLE,
	REFLECT_RENDERABLE,
	RENDERABLE_TYPE_COUNT,
};

class CGameLogic : public ILogic
{
public:
	virtual bool BootUp( IPlatform& platform ) override;
	virtual void Update() override;
	virtual void Pause( ) override;
	virtual void Resume( ) override;
	virtual void HandleUserInput( const UserInput& input ) override;
	virtual void AppSizeChanged( IPlatform& platform ) override;
	
	void SpawnObject( Owner<CGameObject*> object );

	//IRenderer& GetRenderer( ) const { return *m_pRenderer; }
	//CLightManager& GetLightManager( ) { return m_lightManager; }
	//CShadowManager& GetShadowManager( ) { return m_shadowManager; }
	CModelManager& GetModelManager( ) { return m_modelManager; }
	//ImUI& GetUIManager( ) { return m_ui; }
	//RE_HANDLE GetCommonConstantBuffer( int purpose ) { return m_commonConstantBuffer[purpose]; }
	const std::pair<UINT, UINT>& GetAPPSize( ) { return m_appSize; }
	const CTimer& GetTimer( ) const { return m_clock; }

	CPlayer* GetLocalPlayer( );

private:
	void Shutdown( );

	void StartLogic ( );
	void ProcessLogic ( );
	void EndLogic ( );

	bool LoadWorld( const char* filePath );

	void InitView( RenderViewGroup& views );
	void DrawScene( const RenderViewGroup& views );
	void DrawForDebug( );
	void DrawDebugOverlay( );
	void DrawUI( );
	void SceneEnd( );

	void BuildRenderableList( );
	void DrawOpaqueRenderable( );
	void DrawTransparentRenderable( );
	void DrawReflectRenderable( );

	void HandleDeviceLost( );
	bool CreateDeviceDependentResource( );
	bool CreateDefaultFontResource( );

public:
	CGameLogic();
	~CGameLogic();

private:
	HMODULE m_renderCoreDll;

	HWND	m_wndHwnd;
	std::pair<UINT, UINT> m_appSize;

	CTimer m_clock;
	CUserInputBroadCaster m_inputBroadCaster;
	//CPickingManager m_pickingManager;
	CLightManager m_lightManager;
	//CShadowManager m_shadowManager;
	//CSSRManager m_ssrManager;
	//CAtmosphericScatteringManager m_atmosphereManager;
	CModelManager m_modelManager;
	IRenderCore* m_pRenderCore = nullptr;

	std::list<CGameObject*> m_renderableList[RENDERABLE_TYPE_COUNT];

	//RE_HANDLE m_commonConstantBuffer[SHARED_CONSTANT_BUFFER::Count];

	// Immediate Mode UI
	//ImUI m_ui;

	//struct ImUiDrawBuffer
	//{
	//	RE_HANDLE m_buffer = RE_HANDLE::InValidHandle( );
	//	UINT m_prevBufferSize = 0;
	//};

	//ImUiDrawBuffer m_uiDrawBuffer[2];
	//Material m_uiMaterial = INVALID_MATERIAL;
	CXMFLOAT4X4 m_uiProjMat;

	CDebugOverlayManager m_debugOverlay;

	World m_world;
	float m_remainPhysicsSimulateTime = 0.f;

#ifdef DEBUGGING_BY_CONSOLE
	CDebugConsole m_commandConsole;
#endif
};
