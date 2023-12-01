#include "AppConfig/AppConfig.h"

#include "Filesystem/FileSystem.h"
#include "InterfaceFactories.h"

#include <filesystem>
#include <map>
#include <mutex>

namespace
{
	constexpr const char* g_configDir = "./Configs";
	constexpr const char* g_saveDir = "./Saved";

	constexpr const char* g_saveIniPath = "./Saved/user_settings.ini";
}

class AppConfig : public IAppConfig
{
public:
	virtual void BootUp( std::atomic<int32>& workInProgress ) override;
	virtual const ini::Ini* GetDefaultIni( const Name& iniName ) const override;
	virtual const std::vector<ini::Ini>& GetSavedIni() const override;

	virtual const std::vector<IConfig*> GetConfigs() const override;

	virtual void RegisterConfig( IConfig* config ) override;

	virtual void SaveConfigToFile() const override;

private:
	std::mutex m_defaultIniLock;
	std::map<Name, ini::Ini> m_defaultIni;

	std::mutex m_savedIniLock;
	std::vector<ini::Ini> m_savedIni;

	std::vector<IConfig*> m_configs;
};

void AppConfig::BootUp( std::atomic<int32>& workInProgress )
{
	auto filesystem = GetInterface<IFileSystem>();

	for ( const auto& p : std::filesystem::recursive_directory_iterator( g_configDir ) )
	{
		if ( p.is_regular_file() )
		{
			if ( p.path().extension() != std::filesystem::path( ".ini" ) )
			{
				continue;
			}

			FileHandle hConfig = filesystem->OpenFile( p.path().generic_string().c_str() );
			if ( hConfig.IsValid() == false )
			{
				assert( false && "Fail to open config file" );
			}

			uint32 fileSize = filesystem->GetFileSize( hConfig );
			auto buffer = new char[fileSize];

			IFileSystem::IOCompletionCallback ParseIniAsset;
			ParseIniAsset.BindFunctor(
				[this, hConfig, fileName = p.path().filename().stem(), &workInProgress](const char* buffer, uint32 bufferSize)
				{
					ini::Reader reader( buffer, bufferSize );
					auto ini = reader.Parse();
					if ( ini.has_value() )
					{
						std::lock_guard lk( m_defaultIniLock );
						m_defaultIni.emplace( Name( fileName.generic_string().c_str()), std::move(ini.value()));
					}

					--workInProgress;
					GetInterface<IFileSystem>()->CloseFile( hConfig );
				}
			);

			bool result = filesystem->ReadAsync( hConfig, buffer, fileSize, &ParseIniAsset );
			if ( result == false )
			{
				delete[] buffer;
				GetInterface<IFileSystem>()->CloseFile( hConfig );
			}

			++workInProgress;
		}
	}

	if ( std::filesystem::exists( g_saveDir ) )
	{
		for ( const auto& p : std::filesystem::recursive_directory_iterator( g_saveDir ) )
		{
			if ( p.is_regular_file() )
			{
				if ( p.path().extension() != std::filesystem::path( ".ini" ) )
				{
					continue;
				}

				FileHandle hSave = filesystem->OpenFile( p.path().generic_string().c_str() );
				if ( hSave.IsValid() == false )
				{
					assert( false && "Fail to open config file" );
				}

				uint32 fileSize = filesystem->GetFileSize( hSave );
				auto buffer = new char[fileSize];

				IFileSystem::IOCompletionCallback ParseIniAsset;
				ParseIniAsset.BindFunctor(
					[this, hSave, &workInProgress]( const char* buffer, uint32 bufferSize )
					{
						ini::Reader reader( buffer, bufferSize );
						auto ini = reader.Parse();
						if ( ini.has_value() )
						{
							std::lock_guard lk( m_savedIniLock );
							m_savedIni.emplace_back( std::move( ini.value() ) );
						}

						--workInProgress;
						GetInterface<IFileSystem>()->CloseFile( hSave );
					}
				);

				bool result = filesystem->ReadAsync( hSave, buffer, fileSize, &ParseIniAsset );
				if ( result == false )
				{
					delete[] buffer;
					GetInterface<IFileSystem>()->CloseFile( hSave );
				}

				++workInProgress;
			}
		}
	}
}

const ini::Ini* AppConfig::GetDefaultIni( const Name& name ) const
{
	auto found = m_defaultIni.find( name );
	if ( found == std::end( m_defaultIni ) )
	{
		return nullptr;
	}

	return &found->second;
}

const std::vector<ini::Ini>& AppConfig::GetSavedIni() const
{
	return m_savedIni;
}

const std::vector<IConfig*> AppConfig::GetConfigs() const
{
	return m_configs;
}

void AppConfig::RegisterConfig( IConfig* config )
{
	m_configs.push_back( config );
}

void AppConfig::SaveConfigToFile() const
{
	if ( std::filesystem::exists( g_saveDir ) == false )
	{
		std::filesystem::create_directory( g_saveDir );
	}

	std::ofstream outputFile( g_saveIniPath, std::ios::trunc );
	if ( outputFile.good() )
	{
		ini::Ini userSettings;
		for ( const IConfig* config : m_configs )
		{
			ini::Section section = config->GetChangedConfig();
			if ( section.HasValue() )
			{
				userSettings.AddSection( config->GetSectionName(), section );
			}
		}

		outputFile << ini::Writer::ToString( userSettings );
	}
}

IAppConfig* CreateAppConfig()
{
	return new AppConfig();
}

void DestroyAppConfig( Owner<IAppConfig*> pAppConfig )
{
	delete pAppConfig;
}
