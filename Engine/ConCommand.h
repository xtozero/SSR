#pragma once

#include "IConsoleMessage.h"
#include <functional>

class ENGINE_DLL CConCommand : public IConsoleMessage
{
public:
	virtual bool IsValue( ) override { return false; }
	virtual bool IsCommand( ) override { return true; }

	virtual void Execute( ) override;
	virtual String GetDescription( ) override { return m_description; };

	CConCommand( const String& name, const String& description, std::function<void()> function );

private:
	String m_name;
	String m_description;
	std::function<void( )> m_func;
};

#define CON_COMMAND( name, description ) \
	static void confunc_##name( ); \
	static CConCommand command_##name( _T( #name ), _T( description ), confunc_##name ); \
	static void confunc_##name( )