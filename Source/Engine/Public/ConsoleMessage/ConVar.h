#pragma once

#include "IConsoleMessage.h"
#include "SizedTypes.h"

namespace engine
{
	class CConVar : public IConsoleMessage
	{
	public:
		ENGINE_DLL virtual bool IsValue() override { return true; }
		ENGINE_DLL virtual bool IsCommand() override { return false; }

		ENGINE_DLL virtual void Execute() override;
		ENGINE_DLL virtual std::string GetDescription() override { return m_description; }

		ENGINE_DLL CConVar( const std::string& name, const std::string& value, const std::string& description );

		ENGINE_DLL void SetValue( const std::string& newValue );
		ENGINE_DLL const std::string& GetString() { return m_value; }
		ENGINE_DLL int32 GetInteger() { return m_iValue; }
		ENGINE_DLL float GetFloat() { return m_fValue; }
		ENGINE_DLL bool GetBool() { return m_iValue > 0; }

	private:
		std::string m_name;
		std::string m_description;

		std::string m_value;
		int32 m_iValue;
		float m_fValue;
	};
}

#define ConVar( name, initValue, description ) \
	static engine::CConVar name( #name, initValue, description )