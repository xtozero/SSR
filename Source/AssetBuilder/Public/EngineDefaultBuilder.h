#pragma once

#include <filesystem>

struct PathEnvironment;

class EngineDefaultBuilder
{
public:
	void Build( std::set<std::filesystem::path>& outProcessed );

private:
	void CreateBRDFLookUpTexture( std::set<std::filesystem::path>& outProcessed );
};