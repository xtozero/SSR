#pragma once

#include "IConsoleMessage.h"

#include <functional>

namespace engine
{
	class ConsoleCommand : public IConsoleMessage
	{
	public:
		ENGINE_DLL virtual bool IsValue() override { return false; }
		ENGINE_DLL virtual bool IsCommand() override { return true; }

		ENGINE_DLL virtual void Execute() override;
		ENGINE_DLL virtual std::string GetDescription() override { return m_description; };

		ENGINE_DLL ConsoleCommand( const std::string& name, const std::string& description, const std::function<void()>& function );

	private:
		std::string m_name;
		std::string m_description;
		std::function<void()> m_func;
	};
}

#define ConCommand( name, description ) \
	static void confunc_##name( ); \
	static engine::ConsoleCommand command_##name( #name, description, confunc_##name ); \
	static void confunc_##name( )