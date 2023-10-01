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
	bool m_duplicateSourceDirectoryStructure = false;
	std::vector<std::filesystem::path> m_ignorePath;
	const json::Value* m_orignal = nullptr;
};

struct PreprocessingEnvironment : public PathEnvironment
{
	std::filesystem::path m_targetExtension;
	std::filesystem::path m_productExtension;
	bool m_allowOverwrite = false;
};

class ManufactureConfig
{
public:
	void Load()
	{
		json::Reader reader;
		[[maybe_unused]] bool success = reader.Parse( "Config/Environment.json", m_configRaw );
		assert( success );

		m_workingDirectory = std::filesystem::absolute( std::filesystem::current_path() );
		if ( const json::Value* pRootDirectory = m_configRaw.Find( "root_directory" ) )
		{
			m_rootDirectory = pRootDirectory->AsString();
		}

		if ( const json::Value* pPathEnvironments = m_configRaw.Find( "path_environments" ) )
		{
			const json::Value& pathEnvironments = *pPathEnvironments;

			std::vector<const char*> memberNames = pathEnvironments.GetMemberNames();
			for ( const char* memberName : memberNames )
			{
				const json::Value& pathEnvironment = pathEnvironments[memberName];
				PathEnvironment& value = m_pathEnvironments[memberName];

				value.m_orignal = &pathEnvironment;

				if ( const json::Value* pSourcePath = pathEnvironment.Find( "source_path" ) )
				{
					value.m_source = std::filesystem::absolute( pSourcePath->AsString() );
					value.m_source.make_preferred();
				}

				if ( const json::Value* pDestinationPath = pathEnvironment.Find( "destination_path" ) )
				{
					value.m_destination = std::filesystem::absolute( pDestinationPath->AsString() );
					value.m_destination.make_preferred();
				}

				if ( const json::Value* pDuplicateDirectory = pathEnvironment.Find( "duplicate_source_directory_structure" ) )
				{
					value.m_duplicateSourceDirectoryStructure = pDuplicateDirectory->AsBool();
				}

				if ( const json::Value* pIgnorePath = pathEnvironment.Find( "ignore_path" ) )
				{
					const json::Value& ignorePath = *pIgnorePath;
					for ( const auto& path : ignorePath )
					{
						value.m_ignorePath.push_back( std::filesystem::absolute( path.AsString() ) );
						value.m_ignorePath.back().make_preferred();
					}
				}
			}
		}

		if ( const json::Value* pPreprocessingEnvironments = m_configRaw.Find( "preprocessing_environments" ) )
		{
			const json::Value& preprocessingEnvironments = *pPreprocessingEnvironments;

			std::vector<const char*> memberNames = preprocessingEnvironments.GetMemberNames();
			for ( const char* memberName : memberNames )
			{
				const json::Value& pathEnvironment = preprocessingEnvironments[memberName];
				PreprocessingEnvironment& value = m_preprocessingEnvironments[memberName];

				if ( const json::Value* pSourcePath = pathEnvironment.Find( "source_path" ) )
				{
					value.m_source = std::filesystem::absolute( pSourcePath->AsString() );
					value.m_source.make_preferred();
				}

				if ( const json::Value* pDestinationPath = pathEnvironment.Find( "destination_path" ) )
				{
					value.m_destination = std::filesystem::absolute( pDestinationPath->AsString() );
					value.m_destination.make_preferred();
				}

				if ( const json::Value* pExtension = pathEnvironment.Find( "target_extension" ) )
				{
					value.m_targetExtension = pExtension->AsString();
					value.m_targetExtension.make_preferred();
				}

				if ( const json::Value* pExtension = pathEnvironment.Find( "product_extension" ) )
				{
					value.m_productExtension = pExtension->AsString();
					value.m_targetExtension.make_preferred();
				}

				if ( const json::Value* pAllowOverWrite = pathEnvironment.Find( "allow_overwrite" ) )
				{
					value.m_allowOverwrite = pAllowOverWrite->AsBool();
				}
			}
		}
	}

	const std::filesystem::path& RootDirectory() const
	{
		return m_rootDirectory;
	}

	const std::filesystem::path& WorkingDirectory() const
	{
		return m_workingDirectory;
	}

	std::map<std::string, PathEnvironment>& PathEnvironments()
	{
		return m_pathEnvironments;
	}

	std::map<std::string, PreprocessingEnvironment>& PreprocessingEnvironments()
	{
		return m_preprocessingEnvironments;
	}

	bool IsPreprocessingAsset( std::filesystem::path path )
	{
		std::filesystem::path targetExtension = path.extension();
		for ( const auto& [key, value] : m_preprocessingEnvironments )
		{
			if ( targetExtension == value.m_targetExtension )
			{
				return true;
			}
		}

		return false;
	}

	static ManufactureConfig& Instance( )
	{
		static ManufactureConfig environment;
		return environment;
	}

private:
	json::Value m_configRaw;

	std::filesystem::path m_rootDirectory;
	std::filesystem::path m_workingDirectory;

	std::map<std::string, PathEnvironment> m_pathEnvironments;
	std::map<std::string, PreprocessingEnvironment> m_preprocessingEnvironments;
};