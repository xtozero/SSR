#pragma once
#include "AssetBuilder.h"

#include "dxcapi.h"

#include <d3dcompiler.h>
#include <set>
#include <vector>
#include <wrl/client.h>

namespace rendercore
{
	class StaticShaderSwitches;
}

class ShaderCompileResult
{
public:
	bool Succeeded() const;
	const char* GetErrorMessage() const;

	ShaderCompileResult( const Microsoft::WRL::ComPtr<IUnknown>& errorMsgBlob, const char* errorMsg );
	ShaderCompileResult() = default;

private:
	Microsoft::WRL::ComPtr<IUnknown> m_errorMsgBlob;
	const char* m_errorMsg = nullptr;
};

class ShaderBuilder final : public IAssetBuilder
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Build( const PathEnvironment& env, const std::filesystem::path& path ) const override;

	virtual bool Initialize() override;

private:
	std::set<uint32> CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType, const rendercore::StaticShaderSwitches& switches, std::vector<ShaderCompileResult>& outErrorMsgs ) const;
	void CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType, rendercore::StaticShaderSwitches& switches, int32 depth, std::set<uint32>& outCompiledShaderIDs, std::vector<ShaderCompileResult>& outErrorMsgs ) const;

	ShaderCompileResult CompileD3D12Shader( const std::string& shaderFile, const char* featureLevel, const rendercore::StaticShaderSwitches& switches ) const;

	Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler;
};