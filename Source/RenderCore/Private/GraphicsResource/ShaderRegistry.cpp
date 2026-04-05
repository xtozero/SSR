#include "ShaderRegistry.h"

#include "Core/Paths.h"

#include <format>

namespace fs = std::filesystem;

namespace rendercore
{
    const std::vector<ShaderDescriptor>* ShaderRegistry::Find( const std::filesystem::path& filePath ) const
    {
        return Find( ShaderDescriptorHandle( filePath.generic_string().c_str() ) );
    }

    const std::vector<ShaderDescriptor>* ShaderRegistry::Find( uint32 handle ) const
    {
        auto found = m_registry.find( handle );
        if ( found != std::end( m_registry ) )
        {
            return &found->second;
        }

        return nullptr;
    }

    void ShaderRegistry::Register( uint32 descriptorHandle, ShaderDescriptor&& descriptor )
    {
        assert( fs::exists( engine::Paths::GetShaderSourceRootDir() / descriptor.m_filePath )
            && "The specified shader file does not exist." );
        m_registry[descriptorHandle].emplace_back( std::move( descriptor ) );
    }

    ShaderRegister::ShaderRegister( uint32 descriptorHandle, const char* filePath, agl::ShaderType shaderType, const char* entryPoint, ShaderPermutationCreateFunc getPermutationFunc )
    {
        ShaderDescriptor shaderDescriptor = {
            .m_filePath = filePath,
            .m_assetName = GetShaderAssetName( filePath, shaderType ),
            .m_type = shaderType,
            .m_entryPoint = entryPoint,
            .m_createPermutationFunc = getPermutationFunc
        };
        ShaderRegistry::GetInstance().Register( descriptorHandle, std::move( shaderDescriptor ) );
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
