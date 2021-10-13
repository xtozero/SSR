#pragma once

#include "InterfaceFactories.h"
#include "SizedTypes.h"

#include <string>

class IEnumStringMap
{
public:
	virtual void RegisterEnumString( const std::string& enumString, const int32 enumValue ) = 0;
	virtual int32 GetEnum( const std::string& enumName, const int32 defaultValue ) const = 0;

	virtual ~IEnumStringMap( ) = default;
};

void* GetEnumStringMap( );

#define REGISTER_ENUM_STRING( enumValue ) \
	GetInterface<IEnumStringMap>( )->RegisterEnumString( #enumValue, static_cast<int32>( enumValue ) )

template <typename T>
T GetEnum( const std::string& enumString, T defaultValue )
{
	int32 enumValue = GetInterface<IEnumStringMap>( )->GetEnum( enumString, static_cast<int32>( defaultValue ) );
	return static_cast<T>( enumValue );
}