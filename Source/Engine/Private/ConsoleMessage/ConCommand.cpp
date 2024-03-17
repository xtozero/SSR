#include "ConCommand.h"

#include "ConsoleMessageExecutor.h"
#include "DebugUtil.h"

namespace engine
{
	void CConCommand::Execute()
	{
		if ( m_func )
		{
			m_func();
		}
	}

	CConCommand::CConCommand( const std::string& name, const std::string& description, const std::function<void()>& function )
		: m_name( name )
		, m_description( description )
		, m_func( function )
	{
		GetConsoleMessageExecutor().RegistConsoleMessage( name, this );
	}
}

CON_COMMAND( test, "Test Console Command" )
{
	DebugMsg( "test concommand executed!!\n" );
}