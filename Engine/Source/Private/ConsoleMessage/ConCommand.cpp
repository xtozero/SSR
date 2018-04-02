#include "stdafx.h"
#include "ConsoleMessage/ConCommand.h"

#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "Util.h"

void CConCommand::Execute( )
{
	if ( m_func )
	{
		m_func();
	}
}

CConCommand::CConCommand( const String& name, const String& description, const std::function<void( )>& function ) :
m_name( name ),
m_description( description ),
m_func( function )
{
	ConsoleMessageExecutor::GetInstance( )->RegistConsoleMessage( name, this );
}

CON_COMMAND( test, "Test Console Command" )
{
	DebugMsg( "test concommand executed!!\n" );
}