#pragma once

#include "GraphicsApiResource.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace rendercore
{
    struct ShaderDescriptor
    {
        std::string_view m_filePath;
        std::string m_assetName;
        agl::ShaderType m_type = agl::ShaderType::None;
        std::string_view m_entryPoint;
    };

    class IShaderRegistry
    {
    public:
        virtual const std::vector<ShaderDescriptor>* Find( const std::filesystem::path& filePath ) const = 0;

        virtual ~IShaderRegistry() = default;
    };

    class ShaderRegistry : public IShaderRegistry
    {
    public:
        static ShaderRegistry& GetInstance()
        {
            static ShaderRegistry instance;
            return instance;
        }

        virtual const std::vector<ShaderDescriptor>* Find( const std::filesystem::path& filePath ) const override;

    private:
        friend class ShaderRegister;

        void Register( ShaderDescriptor&& descriptor );
        std::unordered_map<std::filesystem::path, std::vector<ShaderDescriptor>> m_registry;
    };

    class ShaderRegister final
    {
    public:
        ShaderRegister( const char* filePath, agl::ShaderType shaderType, const char* entryPoint );
    };

    std::string GetShaderAssetName( const char* filePath, agl::ShaderType shaderType );

    #define REGISTER_SHADER( shaderClass, filePath, entryPoint ) \
    static_assert( HasShaderType<shaderClass>, \
    "ShaderClass must define 'static constexpr agl::ShaderType Type'. " \
    "Check if it correctly inherits from ShaderTraits or its equivalents.(ex GlobalShaderBase)" ); \
    static ShaderRegister shaderClass##_register( filePath, shaderClass::Type, entryPoint )
}
