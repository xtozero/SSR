#pragma once

#include "Camera.h"
#include "common.h"
#include <memory>
#include "MouseController.h"
#include "SceneLoader.h"
#include "Timer.h"
#include <vector>

class CGameObject;

class LOGIC_DLL CGameLogic
{
private:
	void StartLogic ( void );
	void ProcessLogic ( void );
	void EndLogic ( void );

	bool InitShaders( void );
	bool LoadScene( void );

	void SceneBegin( void );
	void DrawScene( void );
	void SceneEnd( void );

	void UpdateWorldMatrix( const CGameObject* object );
	void InitCameraProperty( std::shared_ptr<KeyValueGroup> keyValue );
public:
	bool Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight );
	void UpdateLogic ( void );
	bool HandleWindowMessage( const MSG& msg );
	void HandleWIndowKeyInput( const int message, const WPARAM wParam, const LPARAM lParam );
	void HandleWIndowMouseInput( const int message, const WPARAM wParam, const LPARAM lParam );

	CGameLogic ();
	~CGameLogic ();

private:
	CCamera m_mainCamera;
	CSceneLoader m_sceneLoader;
	std::vector<std::shared_ptr<CGameObject>> m_gameObjects;
	CMouseController m_mouseController;
};

