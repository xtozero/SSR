#pragma once

#include "common.h"

class LOGIC_DLL CGameLogic
{
private:
	void StartLogic ( void );
	void ProcessLogic ( void );
	void EndLogic ( void );

public:
	bool Initialize ( HWND hwnd, UINT wndWidth, UINT wndHeight );
	void UpdateLogic ( void );

	CGameLogic ();
	~CGameLogic ();
};

