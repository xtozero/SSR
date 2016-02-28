#include "stdafx.h"
#include "ConsoleMessageExecutor.h"
#include "../shared/Util.h"

IConsoleMessageExecutor* GetConsoleMessageExecutor( )
{
	return ConsoleMessageExecutor::GetInstance( );
}

void ConsoleMessageExecutor::RegistConsoleMessage( const String& name, IConsoleMessage* consoleMessage )
{
	m_consoleMessages.emplace( name, consoleMessage );
}

void ConsoleMessageExecutor::UnRegistConsoleMessage( const String& name )
{
	auto found = m_consoleMessages.find( name );

	if ( found != m_consoleMessages.end( ) )
	{
		m_consoleMessages.erase( found );
	}
}

void ConsoleMessageExecutor::Execute( const String& cmdString )
{
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

void ConsoleMessageExecutor::PrintConsoleMessages( )
{
	FOR_EACH_MAP( m_consoleMessages, i )
	{
		if ( i->second == nullptr )
		{
			return;
		}
		
		DebugMsg( "%s - %s\n", i->first.c_str( ), i->second->GetDescription( ).c_str( ) );
	}
}

ConsoleMessageExecutor::ConsoleMessageExecutor( ) :
m_argC( 0 )
{
}

void ConsoleMessageExecutor::TokenizingCmdString( const String& cmdString )
{
	m_argV.clear( );
	UTIL::Split( cmdString, m_argV, _T( ' ' ) );
	m_argC = m_argV.size( );
}

CON_COMMAND( list, "List of Registed Console Messages" )
{
	ConsoleMessageExecutor::GetInstance( )->PrintConsoleMessages( );
}

CON_COMMAND( exit, "Terminate Process" )
{
	::exit( 0 );
}