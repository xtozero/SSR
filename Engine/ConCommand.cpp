#include "stdafx.h"
#include "ConCommand.h"
#include "ConsoleMessageExecutor.h"
#include "../shared/Util.h"


void CConCommand::Execute( )
{
	if ( m_func._Empty() )
	{
		return;
	}

	m_func( );
}

CConCommand::CConCommand( const String& name, const String& description, std::function<void( )> function ) :
m_name( name ),
m_description( description ),
m_func( function )
{
	ConsoleMessageExecutor::GetInstance( )->RegistConsoleMessage( name, this );
}

CON_COMMAND( test, "Test Console Command" )
{
	DebugMsg( _T( "test concommand executed!!\n" ) );
}