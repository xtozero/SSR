#pragma once

#include <filesystem>

class PathEnvironment
{
public:
	const std::filesystem::path& SrcAssetRoot( ) const
	{
		return m_srcAssetRoot;
	}

	const std::filesystem::path& DestAssetRoot( ) const
	{
		return m_destAssetRoot;
	}

	void Initialize( const std::filesystem::path& srcAssetRoot, const std::filesystem::path& destAssetRoot )
	{
		m_srcAssetRoot = srcAssetRoot;
		m_destAssetRoot = destAssetRoot;
	}

	static PathEnvironment& Instance( )
	{
		static PathEnvironment environment;
		return environment;
	}

private:
	std::filesystem::path m_srcAssetRoot;
	std::filesystem::path m_destAssetRoot;
};