#include "stdafx.h"
#include "ConsoleMessage/ConVar.h"

#include "ConsoleMessage/ConsoleMessageExecutor.h"
#include "Util.h"

#include <cstddef>
#include <tchar.h>
#include <vector>

void CConVar::Execute( )
{
	std::size_t argC = GetConsoleMessageExecutor( ).ArgC( );

	if ( argC == 1 )
	{
		DebugMsg( "%s - %s\n", m_name.c_str( ), m_value.c_str( ) );
	}
	else
	{
		const std::vector<String>& argV = GetConsoleMessageExecutor( ).ArgV( );
		SetValue( argV[1] );
	}
}

void CConVar::SetValue( const String& newValue )
{
	m_value = newValue;
	m_iValue = _ttoi( newValue.c_str( ) );
	m_fValue = static_cast<float>( _ttof( newValue.c_str( ) ) );
}

CConVar::CConVar( const String& name, const String& value, const String& description ) :
m_name( name ),
m_description( description )
{
	SetValue( value );
	GetConsoleMessageExecutor( ).RegistConsoleMessage( name, this );
}
