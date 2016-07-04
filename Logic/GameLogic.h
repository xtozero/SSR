#pragma once

#include "Camera.h"
#include "common.h"
#include "LightManager.h"
#include "MouseController.h"
#include "PickingManager.h"
#include "SceneLoader.h"
#include "ShadowManager.h"
#include "Timer.h"

#include <memory>
#include <vector>

class CGameObject;
std::vector<std::shared_ptr<CGameObject>> g_gameObjects;

class LOGIC_DLL CGameLogic
{
private:
	void StartLogic ( void );
	void ProcessLogic ( void );
	void EndLogic ( void );

	bool InitShaders( void );
	bool LoadScene( void );

	void SceneBegin( void ) const;
	void DrawScene( void ) const;
	void SceneEnd( void ) const;

	void UpdateWorldMatrix( CGameObject* object ) const;
	void InitCameraProperty( std::shared_ptr<KeyValueGroup> keyValue );
public:
	bool Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight );
	void UpdateLogic ( void );
	bool HandleWindowMessage( const MSG& msg );
	void HandleWIndowKeyInput( const int message, const WPARAM wParam, const LPARAM lParam );
	void HandleWIndowMouseInput( const int message, const WPARAM wParam, const LPARAM lParam );

	CGameLogic ();
	~CGameLogic () = default;

private:
	HWND	m_wndHwnd;
	CCamera m_mainCamera;
	CSceneLoader m_sceneLoader;
	CMouseController m_mouseController;
	CPickingManager m_pickingManager;
	CLightManager m_lightManager;
	CShadowManager m_shadowManager;
};

