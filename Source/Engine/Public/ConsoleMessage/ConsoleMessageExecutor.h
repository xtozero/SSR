#pragma once

#include "IConsoleMessage.h"
#include "SizedTypes.h"

#include <map>
#include <mutex>
#include <vector>

namespace engine
{
	class IConsoleMessageExecutor
	{
	public:
		virtual void RegistConsoleMessage( const std::string& name, IConsoleMessage* consoleMessage ) = 0;
		virtual void UnRegistConsoleMessage( const std::string& name ) = 0;
		virtual void AppendCommand( std::string&& command ) = 0;
		virtual void Execute() = 0;

		virtual const std::vector<std::string>& ArgV() const = 0;
		virtual size_t ArgC() const = 0;

		virtual const std::map<std::string, IConsoleMessage*>& GetConsoleMessages() const = 0;

		static IConsoleMessageExecutor& GetInstance();

		virtual ~IConsoleMessageExecutor() = default;
	};
}
