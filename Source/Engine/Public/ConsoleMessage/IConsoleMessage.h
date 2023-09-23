#pragma once

#include "common.h"

namespace engine
{
	class ENGINE_DLL IConsoleMessage
	{
	public:
		virtual bool IsValue() = 0;
		virtual bool IsCommand() = 0;

		virtual void Execute() = 0;
		virtual std::string GetDescription() = 0;

		virtual ~IConsoleMessage() = default;

	protected:
		IConsoleMessage() = default;
	};
}