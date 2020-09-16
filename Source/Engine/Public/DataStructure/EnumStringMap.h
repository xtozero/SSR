#pragma once

#include "common.h"
#include "Core/InterfaceFactories.h"

#include <string>

class IEnumStringMap
{
public:
	virtual void RegisterEnumString( const std::string& enumString, const int enumValue ) = 0;
	virtual int GetEnum( const std::string& enumName, const int defaultValue ) const = 0;

	virtual ~IEnumStringMap( ) = default;
};

void* GetEnumStringMap( );

#define REGISTER_ENUM_STRING( enumValue ) \
	GetInterface<IEnumStringMap>( )->RegisterEnumString( #enumValue, static_cast<int>( enumValue ) )

template <typename T>
T GetEnum( const std::string& enumString, T defaultValue )
{
	int enumValue = GetInterface<IEnumStringMap>( )->GetEnum( enumString, static_cast<int>( defaultValue ) );
	return static_cast<T>( enumValue );
}