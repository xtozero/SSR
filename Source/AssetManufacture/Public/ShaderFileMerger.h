#pragma once

#include <filesystem>
#include <optional>
#include <set>
#include <string>

class ShaderFileMerger final
{
public:
	std::optional<std::string> Merge( const std::filesystem::path& shaderFile );

	explicit ShaderFileMerger( const std::filesystem::path& includeFilesDirectory );

public:
	void MergeRecursive( const std::filesystem::path& includeFile );

	std::filesystem::path m_includeFileDirectory;
	std::string m_buffer;

	std::set<std::filesystem::path> m_mergedFiles;
};