#pragma once

#include "IConsoleMessage.h"

#include <functional>

namespace engine
{
	class CConCommand : public IConsoleMessage
	{
	public:
		ENGINE_DLL virtual bool IsValue() override { return false; }
		ENGINE_DLL virtual bool IsCommand() override { return true; }

		ENGINE_DLL virtual void Execute() override;
		ENGINE_DLL virtual std::string GetDescription() override { return m_description; };

		ENGINE_DLL CConCommand( const std::string& name, const std::string& description, const std::function<void()>& function );

	private:
		std::string m_name;
		std::string m_description;
		std::function<void()> m_func;
	};
}

#define CON_COMMAND( name, description ) \
	static void confunc_##name( ); \
	static engine::CConCommand command_##name( #name, description, confunc_##name ); \
	static void confunc_##name( )