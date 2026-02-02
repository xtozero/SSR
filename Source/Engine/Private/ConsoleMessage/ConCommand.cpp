#include "ConCommand.h"

#include "ConsoleMessageExecutor.h"
#include "DebugUtil.h"

namespace engine
{
	void ConsoleCommand::Execute()
	{
		if ( m_func )
		{
			m_func();
		}
	}

	ConsoleCommand::ConsoleCommand( const std::string& name, const std::string& description, const std::function<void()>& function )
		: m_name( name )
		, m_description( description )
		, m_func( function )
	{
		IConsoleMessageExecutor::GetInstance().RegistConsoleMessage( name, this );
	}
}
