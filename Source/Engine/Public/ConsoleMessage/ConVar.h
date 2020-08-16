#pragma once

#include "IConsoleMessage.h"

class ENGINE_DLL CConVar : public IConsoleMessage
{
public:
	virtual bool IsValue( ) override { return true; }
	virtual bool IsCommand( ) override { return false; }

	virtual void Execute( ) override;
	virtual std::string GetDescription( ) override { return m_description; }

	CConVar( const std::string& name, const std::string& value, const std::string& description );

	void SetValue( const std::string& newValue );
	const std::string& GetString( ) { return m_value; }
	int GetInteger( ) { return m_iValue; }
	float GetFloat( ) { return m_fValue; }
	bool GetBool( ) { return m_iValue > 0; }

private:
	std::string m_name;
	std::string m_description;

	std::string m_value;
	int m_iValue;
	float m_fValue;
};

#define ConVar( name, initValue, description ) \
	static CConVar name( #name, initValue, description )