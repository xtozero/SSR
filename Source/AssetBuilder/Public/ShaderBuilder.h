#pragma once
#include "AssetBuilder.h"

#include "dxcapi.h"

#include <vector>
#include <wrl/client.h>

class ShaderCompileResult
{
public:
	bool Succeeded() const;
	const char* GetErrorMessage() const;

	ShaderCompileResult( bool succeeded, const Microsoft::WRL::ComPtr<IUnknown>& errorMsgBlob );
	ShaderCompileResult() = default;

private:
	Microsoft::WRL::ComPtr<IUnknown> m_errorMsgBlob;
	bool m_succeeded = false;
};

class ShaderBuilder final : public IAssetBuilder
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Build( const PathEnvironment& env, const std::filesystem::path& path ) const override;

	virtual bool Initialize() override;

private:
	bool CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType,  const char* entryPoint, rendercore::IShaderPermutation& permutation, std::vector<ShaderCompileResult>& outErrorMsgs ) const;

	ShaderCompileResult CompileD3D12Shader( const std::string& shaderFile,  const char* entryPoint, const char* featureLevel, const rendercore::IShaderPermutation& permutation ) const;

	Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler;
};