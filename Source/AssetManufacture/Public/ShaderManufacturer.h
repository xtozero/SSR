#pragma once
#include "AssetManufacturer.h"

#include <map>
#include <vector>

namespace rendercore
{
	class StaticShaderSwitches;
}

class ShaderManufacturer : public IManufacturer
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Manufacture( const PathEnvironment& env, const std::filesystem::path& path ) const override;

private:
	void CombinationStaticSwitches( const std::string& shaderFile, const char* featureLevel, std::map<size_t, void*>& outCompiledShaders, const rendercore::StaticShaderSwitches& switches ) const;
	void CombinationStaticSwitchesRecursive( const std::string& shaderFile, const char* featureLevel, std::map<size_t, void*>& outCompiledShaders, rendercore::StaticShaderSwitches& switches, int depth ) const;
};