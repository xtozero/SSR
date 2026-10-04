#pragma once

#include "AssetBuilder.h"
#include "ShaderCompiler/ShaderCompiler.h"

#include <dxcapi.h>
#include <vector>
#include <wrl/client.h>

class ShaderBuilder final : public IAssetBuilder
{
public:
	virtual bool IsSuitable( const std::filesystem::path& srcPath ) const override;
	virtual std::optional<Products> Build( const PathEnvironment& env, const std::filesystem::path& path ) const override;

	virtual bool Initialize() override;

private:
	bool CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType, const char* entryPoint, rendercore::IShaderPermutation& permutation, std::vector<BinaryChunk>& outErrorMsgs ) const;

	agl::ShaderCompileResult CompileD3D11Shader( const BinaryChunk& source, const std::vector<const char*>& defines, agl::ShaderType shaderType, const char* entryPoint ) const;
	agl::ShaderCompileResult CompileD3D12Shader( const BinaryChunk& source, const std::vector<const char*>& defines, agl::ShaderType shaderType, const char* entryPoint ) const;

	agl::ShaderCompiler m_shaderCompiler;
};