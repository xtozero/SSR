#pragma once

#include "Archive.h"

#include "dxcapi.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace agl
{
    class ShaderCompileResult
    {
    public:
        AGL_DLL bool Succeeded() const;

        const BinaryChunk& GetByteCode() const;
        AGL_DLL const BinaryChunk& GetErrorMessage() const;

    private:
        friend class ShaderCompiler;

        BinaryChunk m_byteCode;
        BinaryChunk m_errorMessage;
    };

    class ShaderCompiler
    {
    public:
        AGL_DLL bool Initialize( D3D_SHADER_MODEL shaderModel );
        AGL_DLL ShaderCompileResult CompileToDXIL( const BinaryChunk& source, const std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const;
        AGL_DLL ShaderCompileResult CompileToDXBC( const BinaryChunk& source, const std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const;

        AGL_DLL static bool CanCompileToDXBC( agl::ShaderType shaderType );

    private:
        const wchar_t* GetShaderProfileForDXIL( ShaderType type ) const;
        const char* GetShaderProfileForDXBC( ShaderType type ) const;

        D3D_SHADER_MODEL m_shaderModel = D3D_SHADER_MODEL_NONE;
        Microsoft::WRL::ComPtr<IDxcCompiler3> m_compiler;
    };
}