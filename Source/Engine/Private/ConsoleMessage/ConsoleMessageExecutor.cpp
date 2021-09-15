#include "stdafx.h"
#include "ConsoleMessage/ConsoleMessageExecutor.h"

#include "Util.h"

#include <cstddef>

class ConsoleMessageExecutor : public IConsoleMessageExecutor
{
public:
	virtual void RegistConsoleMessage( const std::string& name, IConsoleMessage* consoleMessage ) override
	{
		m_consoleMessages.emplace( name, consoleMessage );
	}

	virtual void UnRegistConsoleMessage( const std::string& name ) override
	{
		auto found = m_consoleMessages.find( name );

		if ( found != m_consoleMessages.end( ) )
		{
			m_consoleMessages.erase( found );
		}
	}

	virtual void AppendCommand( std::string&& command ) override
	{
		std::lock_guard<std::mutex> lock( m_commandStackMutex );
		m_commandStack.push( command );
	}

	virtual void Execute( ) override
	{
		if ( m_commandStack.size( ) == 0 )
		{
			return;
		}

		std::string cmdString;
		{
			std::lock_guard<std::mutex> lock( m_commandStackMutex );
			cmdString = std::move( m_commandStack.top( ) );
			m_commandStack.pop( );
		}

		TokenizingCmdString( cmdString );

		if ( m_argC == 0 )
		{
			return;
		}

		auto found = m_consoleMessages.find( m_argV[0] );

		if ( found != m_consoleMessages.end( ) )
		{
			if ( found->second == nullptr )
			{
				return;
			}

			found->second->Execute( );
		}
	}

	void PrintConsoleMessages( )
	{
		for( const auto& message : m_consoleMessages )
		{
			if ( message.second == nullptr )
			{
				return;
			}

			DebugMsg( "%s - %s\n", message.first.c_str( ), message.second->GetDescription( ).c_str( ) );
		}
	}

	virtual const std::vector<std::string>& ArgV( ) const override { return m_argV; }
	virtual size_t ArgC( ) const override { return m_argC; }

private:
	void TokenizingCmdString( const std::string& cmdString )
	{
		m_argV.clear( );
		UTIL::Split( cmdString, m_argV, ' ' );
		m_argC = m_argV.size( );
	}

	std::map<std::string, IConsoleMessage*> m_consoleMessages;

	std::vector<std::string> m_argV;
	size_t m_argC = 0;

	std::stack<std::string> m_commandStack;
	std::mutex m_commandStackMutex;
};

IConsoleMessageExecutor& GetConsoleMessageExecutor( )
{
	static ConsoleMessageExecutor g_consoleMessageExecutor;
	return g_consoleMessageExecutor;
}

CON_COMMAND( list, "List of Registed Console Messages" )
{
	static_cast<ConsoleMessageExecutor&>( GetConsoleMessageExecutor( ) ).PrintConsoleMessages( );
}

CON_COMMAND( exit, "Terminate Process" )
{
	::exit( 0 );
}