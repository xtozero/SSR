#include "ShaderRegistry.h"

#include "Core/Paths.h"

#include <format>

namespace fs = std::filesystem;

namespace rendercore
{
    const std::vector<ShaderDescriptor>* ShaderRegistry::Find( const std::filesystem::path& filePath ) const
    {
        auto found = m_registry.find( filePath );
        if ( found != std::end( m_registry ) )
        {
            return &found->second;
        }

        return nullptr;
    }

    void ShaderRegistry::Register( ShaderDescriptor&& descriptor )
    {
        assert( fs::exists( engine::Paths::GetShaderSourceRootDir() / descriptor.m_filePath )
            && "The specified shader file does not exist." );
        m_registry[descriptor.m_filePath].emplace_back( std::move( descriptor ) );
    }

    ShaderRegister::ShaderRegister( const char* filePath, agl::ShaderType shaderType, const char* entryPoint )
    {
        ShaderDescriptor shaderDescriptor = {
            .m_filePath = filePath,
            .m_assetName = GetShaderAssetName( filePath, shaderType ),
            .m_type = shaderType,
            .m_entryPoint = entryPoint,
        };
        ShaderRegistry::GetInstance().Register( std::move( shaderDescriptor ) );
    }

    std::string GetShaderAssetName( const char* filePath, agl::ShaderType shaderType )
    {
        std::string_view shaderTypeString( agl::ToString( shaderType ) );
        auto fileName = fs::path( filePath ).filename().generic_string();

        if ( fileName.starts_with( shaderTypeString ) )
        {
            return fileName;
        }

        return std::format( "{}_{}", shaderTypeString, fileName );
    }
}
