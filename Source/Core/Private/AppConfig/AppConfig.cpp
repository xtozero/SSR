#include "AppConfig/AppConfig.h"

#include "Filesystem/FileSystem.h"
#include "InterfaceFactories.h"

#include <filesystem>
#include <map>
#include <mutex>

namespace
{
	constexpr const char* g_configDir = "./Configs";
}

class AppConfig : public IAppConfig
{
public:
	virtual void BootUp( std::atomic<int>& workInProgress );
	virtual const ini::Ini* GetConfig( const Name& configName ) const;

private:
	std::mutex m_configLock;
	std::map<Name, ini::Ini> m_configs;
};

void AppConfig::BootUp( std::atomic<int>& workInProgress )
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

			IFileSystem::IOCompletionCallback ParseUICAsset;
			ParseUICAsset.BindFunctor(
				[this, hConfig, fileName = p.path().filename().stem(), &workInProgress](const char* buffer, uint32 bufferSize)
				{
					ini::IniReader reader( buffer, bufferSize );
					auto ini = reader.Parse();
					if ( ini.has_value() )
					{
						std::lock_guard lk( m_configLock );
						m_configs.emplace( Name( fileName.generic_string().c_str()), std::move(ini.value()));
					}

					--workInProgress;
					GetInterface<IFileSystem>()->CloseFile( hConfig );
				}
			);

			bool result = filesystem->ReadAsync( hConfig, buffer, fileSize, &ParseUICAsset );
			if ( result == false )
			{
				delete[] buffer;
				GetInterface<IFileSystem>()->CloseFile( hConfig );
			}

			++workInProgress;
		}
	}
}

const ini::Ini* AppConfig::GetConfig( const Name& configName ) const
{
	auto found = m_configs.find( configName );
	if ( found == std::end( m_configs ) )
	{
		return nullptr;
	}

	return &found->second;
}

IAppConfig* CreateAppConfig()
{
	return new AppConfig();
}

void DestroyAppConfig( Owner<IAppConfig*> pAppConfig )
{
	delete pAppConfig;
}
