#pragma once

#include "IConsoleMessage.h"

#include <functional>

class ENGINE_DLL CConCommand : public IConsoleMessage
{
public:
	virtual bool IsValue( ) override { return false; }
	virtual bool IsCommand( ) override { return true; }

	virtual void Execute( ) override;
	virtual std::string GetDescription( ) override { return m_description; };

	CConCommand( const std::string& name, const std::string& description, const std::function<void()>& function );

private:
	std::string m_name;
	std::string m_description;
	std::function<void( )> m_func;
};

#define CON_COMMAND( name, description ) \
	static void confunc_##name( ); \
	static CConCommand command_##name( #name, description, confunc_##name ); \
	static void confunc_##name( )