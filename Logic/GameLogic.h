#pragma once

#include "common.h"

class LOGIC_DLL CGameLogic
{
private:
	void StartLogic ( void );
	void ProcessLogic ( void );
	void EndLogic ( void );

public:
	CGameLogic ();
	~CGameLogic ();

	void UpdateLogic ( void );
};

