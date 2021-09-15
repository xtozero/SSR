#include "stdafx.h"
#include "ConsoleMessage/ConVar.h"

#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "Util.h"

#include <cstddef>
#include <tchar.h>
#include <vector>

void CConVar::Execute( )
{
	size_t argC = GetConsoleMessageExecutor( ).ArgC( );

	if ( argC == 1 )
	{
		DebugMsg( "%s - %s\n", m_name.c_str( ), m_value.c_str( ) );
	}
	else
	{
		const std::vector<std::string>& argV = GetConsoleMessageExecutor( ).ArgV( );
		SetValue( argV[1] );
	}
}

void CConVar::SetValue( const std::string& newValue )
{
	m_value = newValue;
	m_iValue = std::atoi( newValue.c_str( ) );
	m_fValue = static_cast<float>( std::atof( newValue.c_str( ) ) );
}

CConVar::CConVar( const std::string& name, const std::string& value, const std::string& description ) :
m_name( name ),
m_description( description )
{
	SetValue( value );
	GetConsoleMessageExecutor( ).RegistConsoleMessage( name, this );
}
