#pragma once

#include "EnumStringMap.h"
#include "GuideTypes.h"
#include "Ini/Ini.h"
#include "NameTypes.h"
#include "Reflection.h"
#include "SizedTypes.h"

#include <atomic>
#include <cassert>

class IAppConfig
{
public:
	virtual void BootUp( std::atomic<int>& workInProgress ) = 0;
	virtual const ini::Ini* GetConfig( const Name& configName ) const = 0;

	virtual ~IAppConfig() = default;
};

template <typename Derived, StringLiteral name>
class ConfigBase
{
public:
	static Derived& GetInstance()
	{
		static Derived config;
		static bool initialized = false;
		if ( initialized == false )
		{
			config.Initialize();
			initialized = true;
		}

		return config;
	}

	void Initialize()
	{
		const ini::Ini* pConfig = GetInterface<IAppConfig>()->GetConfig( Name( name.m_str ) );
		if ( pConfig == nullptr )
		{
			return;
		}

		const TypeInfo& typeInfo = Derived::StaticTypeInfo();
		const std::vector<const Property*>& properties = typeInfo.GetProperties();

		const ini::Section* pSection = pConfig->GetSection( Name( typeInfo.GetName() ) );
		if ( pSection == nullptr )
		{
			return;
		}

		for ( const Property* property : properties )
		{
			const std::string* value = pSection->GetValue( Name( property->GetName() ) );
			if ( value == nullptr )
			{
				continue;
			}

			const TypeInfo& propertyType = property->GetTypeInfo();
			if ( &propertyType == &TypeInfo::GetStaticTypeInfo<std::string>() )
			{
				property->Set( this, *value );
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<bool>() )
			{
				bool boolean = std::strcmp( value->c_str(), "true" ) == 0
							|| std::strcmp( value->c_str(), "True" ) == 0;
				property->Set( this, boolean );
			}
			else if ( std::strstr(value->c_str(), "::") != nullptr )
			{
				if ( &propertyType == &TypeInfo::GetStaticTypeInfo<uint8>() )
				{
					property->Set( this, GetEnum<uint8>( *value, 0 ) );
				}
				else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<uint16>() )
				{
					property->Set( this, GetEnum<uint16>( *value, 0 ) );
				}
				else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<uint32>() )
				{
					property->Set( this, GetEnum<uint32>( *value, 0 ) );
				}
				else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<int8>() )
				{
					property->Set( this, GetEnum<int8>( *value, 0 ) );
				}
				else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<int16>() )
				{
					property->Set( this, GetEnum<int16>( *value, 0 ) );
				}
				else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<int32>() )
				{
					property->Set( this, GetEnum<int32>( *value, 0 ) );
				}
			}
			else
			{
				assert( false && "Invalid property type for config" );
			}
		}
	}

	virtual ~ConfigBase() = default;

protected:
	ConfigBase() = default;
};

IAppConfig* CreateAppConfig();
void DestroyAppConfig( Owner<IAppConfig*> pAppConfig );
