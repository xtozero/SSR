#pragma once

#include "Json/Json.hpp"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

struct PathEnvironment
{
	std::filesystem::path m_source;
	std::filesystem::path m_destination;
	bool m_duplicateSourceDirectoryStructure;
	std::vector<std::filesystem::path> m_ignorePath;
	const JSON::Value* m_orignal;
};

class ManufactureConfig
{
public:
	void Load()
	{
		using namespace JSON;

		Reader reader;
		assert( reader.Parse( "Config/Environment.json", m_configRaw ) );

		m_workingDirectory = std::filesystem::absolute( std::filesystem::current_path() );

		std::vector<const char*> memberNames = m_configRaw.GetMemberNames();
		for ( const char* memberName : memberNames )
		{
			const Value& pathEnvironment = m_configRaw[memberName];
			PathEnvironment& value = m_environments[memberName];

			value.m_orignal = &pathEnvironment;

			if ( const Value* pSourcePath = pathEnvironment.Find( "source_path" ) )
			{
				value.m_source = std::filesystem::absolute( pSourcePath->AsString() );
				value.m_source.make_preferred();
			}

			if ( const Value* pDestinationPath = pathEnvironment.Find( "destination_path" ) )
			{
				value.m_destination = std::filesystem::absolute( pDestinationPath->AsString() );
				value.m_destination.make_preferred();
			}

			if ( const Value* pDuplicateDirectory = pathEnvironment.Find( "duplicate_source_directory_structure" ) )
			{
				value.m_duplicateSourceDirectoryStructure = pDuplicateDirectory->AsBool();
			}

			if ( const Value* pIgnorePath = pathEnvironment.Find( "ignore_path" ) )
			{
				const Value& ignorePath = *pIgnorePath;
				for ( const auto& path : ignorePath )
				{
					value.m_ignorePath.push_back( std::filesystem::absolute( path.AsString() ) );
					value.m_ignorePath.back().make_preferred();
				}
			}
		}
	}

	std::map<std::string, PathEnvironment>& Environments()
	{
		return m_environments;
	}

	const std::filesystem::path& WorkingDirectory() const
	{
		return m_workingDirectory;
	}

	static ManufactureConfig& Instance( )
	{
		static ManufactureConfig environment;
		return environment;
	}

private:
	std::map<std::string, PathEnvironment> m_environments;
	JSON::Value m_configRaw;
	std::filesystem::path m_workingDirectory;
};