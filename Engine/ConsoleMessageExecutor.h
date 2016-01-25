#pragma once

#include "common.h"
#include "ConCommand.h"
#include "IConsoleMessage.h"
#include <functional>
#include <map>
#include <memory>
#include <tchar.h>
#include <vector>

class IConsoleMessageExecutor
{
public:
	virtual void Execute( const String& name ) = 0;
};

class ConsoleMessageExecutor : public IConsoleMessageExecutor
{
public:
	static ConsoleMessageExecutor* GetInstance( )
	{
		static ConsoleMessageExecutor instance;
		return &instance;
	}

	void RegistConsoleMessage( const String& name, IConsoleMessage* consoleMessage );
	void UnRegistConsoleMessage( const String& name );

	virtual void Execute( const String& cmdString ) override;

	void PrintConsoleMessages( );

	std::vector<String>& ArgV( ) { return m_argV; }
	int ArgC( ) { return m_argC; }

	ConsoleMessageExecutor( );

private:
	void TokenizingCmdString( const String& cmdString );

	std::map<String, IConsoleMessage*> m_consoleMessages;

	std::vector<String> m_argV;
	int m_argC;
};

ENGINE_FUNC_DLL IConsoleMessageExecutor* GetConsoleMessageExecutor( );

#define CON_COMMAND( name, description ) \
	void confunc_##name(); \
	CConCommand command_##name( _T( #name ), _T( description ), confunc_##name ); \
	void confunc_##name()