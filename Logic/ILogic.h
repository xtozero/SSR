#pragma once

#include "common.h"
#include "../shared/Util.h"

class IPlatform;

class ILogic
{
public:
	virtual bool Initialize( IPlatform& ) = 0;
	virtual void Update( ) = 0;
};

LOGIC_FUNC_DLL Owner<ILogic*> CreateGameLogic( );
