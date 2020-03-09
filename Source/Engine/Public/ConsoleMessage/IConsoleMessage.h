#pragma once

#include "common.h"

class ENGINE_DLL IConsoleMessage
{
public:
	virtual bool IsValue( ) = 0;
	virtual bool IsCommand( ) = 0;

	virtual void Execute( ) = 0;
	virtual String GetDescription( ) = 0;

	virtual ~IConsoleMessage( ) = default;

protected:
	IConsoleMessage( ) = default;
};