#pragma once
#include "AssetManufacturer.h"

#include <d3dcompiler.h>
#include <map>
#include <vector>
#include <wrl/client.h>

namespace rendercore
{
	class StaticShaderSwitches;
}

class ShaderManufacturer final : public IManufacturer
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Manufacture( const PathEnvironment& env, const std::filesystem::path& path ) const override;

private:
	void CombinationStaticSwitches( const std::string& shaderFile, const char* featureLevel, const rendercore::StaticShaderSwitches& switches, std::map<uint32, Microsoft::WRL::ComPtr<ID3DBlob>>& outCompiledShaders, std::vector<Microsoft::WRL::ComPtr<ID3DBlob>>& outErrorMsgs ) const;
	void CombinationStaticSwitchesRecursive( const std::string& shaderFile, const char* featureLevel, rendercore::StaticShaderSwitches& switches, int32 depth, std::map<uint32, Microsoft::WRL::ComPtr<ID3DBlob>>& outCompiledShaders, std::vector<Microsoft::WRL::ComPtr<ID3DBlob>>& outErrorMsgs ) const;
};