#pragma once

#include "common.h"
#include "ConCommand.h"
#include "IConsoleMessage.h"
#include "SizedTypes.h"

#include <cstddef>
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
	virtual void RegistConsoleMessage( const std::string& name, IConsoleMessage* consoleMessage ) = 0;
	virtual void UnRegistConsoleMessage( const std::string& name ) = 0;
	virtual void AppendCommand( std::string&& command ) = 0;
	virtual void Execute( ) = 0;

	virtual const std::vector<std::string>& ArgV( ) const = 0;
	virtual size_t ArgC( ) const = 0;

	virtual ~IConsoleMessageExecutor( ) = default;
};

ENGINE_FUNC_DLL IConsoleMessageExecutor& GetConsoleMessageExecutor( );