#pragma once

#include "common.h"
#include "Util.h"

class IPlatform;
struct UserInput;

class ILogic
{
public:
	virtual bool Initialize( IPlatform& ) = 0;
	virtual void Update( ) = 0;
	virtual void Pause( ) = 0;
	virtual void Resume( ) = 0;
	virtual void HandleUserInput( const UserInput& ) = 0;
	virtual void AppSizeChanged( IPlatform& platform ) = 0;

	virtual ~ILogic( ) = default;
};

LOGIC_FUNC_DLL Owner<ILogic*> CreateGameLogic( );
