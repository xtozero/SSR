#include "ConVar.h"

#include "ConsoleMessageExecutor.h"
#include "LogMessage.h"

#include <vector>

namespace
{
	engine::ConVariable ConVarTest( "convarTest", "1", "Console variable for test" );
}

namespace engine
{
	void ConVariable::Execute()
	{
		auto& consoleMessageExecutor = IConsoleMessageExecutor::GetInstance();
		size_t argC = consoleMessageExecutor.ArgC();

		if ( argC == 1 )
		{
			ILogMessage::GetInstance().Log( "{} - {}", m_name, m_value );
		}
		else
		{
			const std::vector<std::string>& argV = consoleMessageExecutor.ArgV();
			SetValue( argV[1] );
		}
	}

	void ConVariable::SetValue( const std::string& newValue )
	{
		m_value = newValue;
		m_iValue = std::atoi( newValue.c_str() );
		m_fValue = static_cast<float>( std::atof( newValue.c_str() ) );
	}

	ConVariable::ConVariable( const std::string& name, const std::string& value, const std::string& description ) :
		m_name( name ),
		m_description( description )
	{
		SetValue( value );
		IConsoleMessageExecutor::GetInstance().RegistConsoleMessage( name, this );
	}
}
