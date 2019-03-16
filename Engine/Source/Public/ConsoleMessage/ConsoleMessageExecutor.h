#pragma once

#include "common.h"
#include "ConCommand.h"
#include "IConsoleMessage.h"

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <stack>
#include <tchar.h>
#include <vector>

class IConsoleMessageExecutor
{
public:
	virtual void RegistConsoleMessage( const String& name, IConsoleMessage* consoleMessage ) = 0;
	virtual void UnRegistConsoleMessage( const String& name ) = 0;
	virtual void AppendCommand( String&& command ) = 0;
	virtual void Execute( ) = 0;

	virtual const std::vector<String>& ArgV( ) const = 0;
	virtual int ArgC( ) const = 0;

	virtual ~IConsoleMessageExecutor( ) = default;
};

ENGINE_FUNC_DLL IConsoleMessageExecutor& GetConsoleMessageExecutor( );