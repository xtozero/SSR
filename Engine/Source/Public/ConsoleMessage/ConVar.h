#pragma once

#include "IConsoleMessage.h"

class ENGINE_DLL CConVar : public IConsoleMessage
{
public:
	virtual bool IsValue( ) override { return true; }
	virtual bool IsCommand( ) override { return false; }

	virtual void Execute( ) override;
	virtual String GetDescription( ) override { return m_description; }

	CConVar( const String& name, const String& value, const String& description );

	void SetValue( const String& newValue );
	const String& GetString( ) { return m_value; }
	int GetInteger( ) { return m_iValue; }
	float GetFloat( ) { return m_fValue; }
	bool GetBool( ) { return m_iValue > 0; }

private:
	String m_name;
	String m_description;

	String m_value;
	int m_iValue;
	float m_fValue;
};

#define ConVar( name, initValue, description ) \
	static CConVar name( _T( #name ), _T( initValue ), _T( description ) )