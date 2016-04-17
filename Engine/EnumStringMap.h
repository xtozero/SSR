#pragma once

#include "common.h"

class IEnumStringMap
{
public:
	virtual void RegisterEnumString( const String& enumString, const int enumValue ) = 0;
	virtual int GetEnum( const String& enumName, const int defaultValue ) const = 0;
};

ENGINE_FUNC_DLL IEnumStringMap& GetEnumStringMap( );

#define REGISTER_ENUM_STRING( enumValue ) \
	GetEnumStringMap().RegisterEnumString( _T( #enumValue ), static_cast<int>( enumValue ) )