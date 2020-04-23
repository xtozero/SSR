#pragma once

#include "common.h"
#include "Core/InterfaceFactories.h"

class IEnumStringMap
{
public:
	virtual void RegisterEnumString( const String& enumString, const int enumValue ) = 0;
	virtual int GetEnum( const String& enumName, const int defaultValue ) const = 0;

	virtual ~IEnumStringMap( ) = default;
};

void* GetEnumStringMap( );

#define REGISTER_ENUM_STRING( enumValue ) \
	GetInterface<IEnumStringMap>( )->RegisterEnumString( _T( #enumValue ), static_cast<int>( enumValue ) )