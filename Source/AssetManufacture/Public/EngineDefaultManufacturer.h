#pragma once

#include <filesystem>

struct PathEnvironment;

class EngineDefaultManufacturer
{
public:
	void Manufacture( std::set<std::filesystem::path>& outProcessed );

private:
	void CreateBRDFLookUpTexture( std::set<std::filesystem::path>& outProcessed );
};