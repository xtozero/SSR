#pragma once

#include "EnumStringMap.h"
#include "GuideTypes.h"
#include "Ini/Ini.h"
#include "NameTypes.h"
#include "Reflection.h"
#include "SizedTypes.h"

#include <atomic>
#include <cassert>

class IConfig
{
public:
	virtual ini::Section GetChangedConfig() const = 0;
	virtual Name GetName() const = 0;
	virtual Name GetSectionName() const = 0;
};

class IAppConfig
{
public:
	virtual void BootUp( std::atomic<int32>& workInProgress ) = 0;
	virtual const ini::Ini* GetDefaultIni( const Name& name ) const = 0;
	virtual const std::vector<ini::Ini>& GetSavedIni() const = 0;

	virtual const std::vector<IConfig*> GetConfigs() const = 0;

	virtual void RegisterConfig( IConfig* config ) = 0;

	virtual void SaveConfigToFile() const = 0;

	virtual ~IAppConfig() = default;
};

template <typename Derived, StringLiteral name>
class ConfigBase : public IConfig
{
public:
	static Derived& GetInstance()
	{
		static Derived config = GetDefault();
		std::call_once( m_initSavedFlag, 
			[]()
			{ 
				config.ReadSavedConfig();
				GetInterface<IAppConfig>()->RegisterConfig( &config );
			} );
		return config;
	}

	static Derived& GetDefault()
	{
		static Derived config;
		std::call_once( m_initDefaultFlag, [](){ config.ReadDefaultConfig(); } );
		return config;
	}

	virtual ini::Section GetChangedConfig() const override final
	{
		ini::Section section;

		const TypeInfo& typeInfo = Derived::StaticTypeInfo();
		const std::vector<const Property*>& properties = typeInfo.GetProperties();

		for ( const Property* property : properties )
		{
			const TypeInfo& propertyType = property->GetTypeInfo();
			if ( &propertyType == &TypeInfo::GetStaticTypeInfo<std::string>() )
			{
				const auto& defaultValue = property->Get<std::string>( &GetDefault() );
				const auto& savedValue = property->Get<std::string>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<bool>() )
			{
				const auto& defaultValue = property->Get<bool>( &GetDefault() );
				const auto& savedValue = property->Get<bool>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() ? "True" : "False" );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<uint8>() )
			{
				const auto& defaultValue = property->Get<uint8>( &GetDefault() );
				const auto& savedValue = property->Get<uint8>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<uint16>() )
			{
				const auto& defaultValue = property->Get<uint16>( &GetDefault() );
				const auto& savedValue = property->Get<uint16>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<uint32>() )
			{
				const auto& defaultValue = property->Get<uint32>( &GetDefault() );
				const auto& savedValue = property->Get<uint32>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<int8>() )
			{
				const auto& defaultValue = property->Get<int8>( &GetDefault() );
				const auto& savedValue = property->Get<int8>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<int16>() )
			{
				const auto& defaultValue = property->Get<int16>( &GetDefault() );
				const auto& savedValue = property->Get<int16>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<int32>() )
			{
				const auto& defaultValue = property->Get<int32>( &GetDefault() );
				const auto& savedValue = property->Get<int32>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else if ( &propertyType == &TypeInfo::GetStaticTypeInfo<float>() )
			{
				const auto& defaultValue = property->Get<float>( &GetDefault() );
				const auto& savedValue = property->Get<float>( &GetInstance() );
				if ( defaultValue != savedValue )
				{
					section.AddValue( Name( property->GetName() ), savedValue.Get() );
				}
			}
			else
			{
				assert( propertyType.IsArray() && "ConfigBase<Derived, name>::GetChangedConfig - Unhandled type" );
			}
		}

		return section;
	}

	virtual Name GetName() const override final
	{
		return Name( name.m_str );
	}

	virtual Name GetSectionName() const override final
	{
		const TypeInfo& typeInfo = Derived::StaticTypeInfo();
		return Name( typeInfo.GetName() );
	}

	virtual ~ConfigBase() = default;

protected:
	ConfigBase() = default;

private:
	void ReadDefaultConfig()
	{
		const TypeInfo& typeInfo = Derived::StaticTypeInfo();

		if ( const ini::Ini* pConfig = GetInterface<IAppConfig>()->GetDefaultIni( Name( name.m_str ) ) )
		{
			const ini::Section* pSection = pConfig->GetSection( Name( typeInfo.GetName() ) );
			if ( pSection != nullptr )
			{
				ReadFromIniSection( pSection );
			}
		}
	}

	void ReadSavedConfig()
	{
		const TypeInfo& typeInfo = Derived::StaticTypeInfo();

		const std::vector<ini::Ini>& saved = GetInterface<IAppConfig>()->GetSavedIni();
		for ( const ini::Ini& ini : saved )
		{
			const ini::Section* pSection = ini.GetSection( Name( typeInfo.GetName() ) );
			if ( pSection != nullptr )
			{
				ReadFromIniSection( pSection );
			}
		}
	}

	void ReadFromIniSection( const ini::Section* pSection )
	{
		const TypeInfo& typeInfo = Derived::StaticTypeInfo();
		const std::vector<const Property*>& properties = typeInfo.GetProperties();

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
			else if ( std::strstr( value->c_str(), "::" ) != nullptr )
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
				property->Parse( this, *value );
			}
		}
	}

	static std::once_flag m_initDefaultFlag;
	static std::once_flag m_initSavedFlag;
};

template <typename Derived, StringLiteral name>
std::once_flag ConfigBase<Derived, name>::m_initDefaultFlag;

template <typename Derived, StringLiteral name>
std::once_flag ConfigBase<Derived, name>::m_initSavedFlag;

IAppConfig* CreateAppConfig();
void DestroyAppConfig( Owner<IAppConfig*> pAppConfig );
