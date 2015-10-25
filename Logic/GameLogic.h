#pragma once

#include "Camera.h"
#include "common.h"

class LOGIC_DLL CGameLogic
{
private:
	void StartLogic ( void );
	void ProcessLogic ( void );
	void EndLogic ( void );

	bool InitShaders( void );
	bool InitModel( void );

	void RenderModel( void );
public:
	bool Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight );
	void UpdateLogic ( void );
	bool HandleWindowMessage( const MSG& msg );
	void HandleWIndowKeyInput( const int message, const WPARAM wParam, const LPARAM lParam );

	CGameLogic ();
	~CGameLogic ();

private:
	CCamera m_mainCamera;
};

