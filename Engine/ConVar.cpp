#include "stdafx.h"
#include "ConVar.h"
#include "ConsoleMessageExecutor.h"
#include "../shared/Util.h"
#include <tchar.h>
#include <vector>

void CConVar::Execute( )
{
	int argC = ConsoleMessageExecutor::GetInstance( )->ArgC( );

	if ( argC == 1 )
	{
		DebugMsg( _T( "%s - %s\n" ), m_name.c_str( ), m_value.c_str( ) );
	}
	else
	{
		const std::vector<String>& argV = ConsoleMessageExecutor::GetInstance( )->ArgV( );
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
	ConsoleMessageExecutor::GetInstance( )->RegistConsoleMessage( name, this );
}


CConVar::~CConVar( )
{
}

ConVar( testConvar, "0", "Test Convar" );