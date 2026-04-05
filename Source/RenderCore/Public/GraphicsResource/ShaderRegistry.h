#pragma once

#include "GraphicsApiResource.h"
#include "Platform/CommandLine.h"
#include "ShaderPermutation.h"

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace rendercore
{
    using ShaderPermutationCreateFunc = std::shared_ptr<IShaderPermutation> (*)();

    struct ShaderDescriptor
    {
        std::string_view m_filePath;
        std::string m_assetName;
        agl::ShaderType m_type = agl::ShaderType::None;
        std::string_view m_entryPoint;
        ShaderPermutationCreateFunc m_createPermutationFunc = nullptr;
    };

    class IShaderRegistry
    {
    public:
        virtual const std::vector<ShaderDescriptor>* Find( const std::filesystem::path& filePath ) const = 0;
        virtual const std::vector<ShaderDescriptor>* Find( uint32 handle ) const = 0;

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
        virtual const std::vector<ShaderDescriptor>* Find( uint32 handle ) const override;

    private:
        friend class ShaderRegister;

        void Register( uint32 descriptorHandle, ShaderDescriptor&& descriptor );
        std::unordered_map<uint32, std::vector<ShaderDescriptor>> m_registry;
    };

    class ShaderRegister final
    {
    public:
        ShaderRegister( uint32 descriptorHandle, const char* filePath, agl::ShaderType shaderType, const char* entryPoint, ShaderPermutationCreateFunc getPermutationFunc );
    };

    std::string GetShaderAssetName( const char* filePath, agl::ShaderType shaderType );

    constexpr uint32 ShaderDescriptorHandle( const char* str )
    {
        return Fnv1a32( str );
    }

    #define REGISTER_SHADER( shaderClass, filePath, entryPoint ) \
    static_assert( HasShaderType<shaderClass>, \
    "ShaderClass must define 'static constexpr agl::ShaderType Type'. " \
    "Check if it correctly inherits from ShaderTraits or its equivalents.(ex GlobalShaderBase)" ); \
    static ShaderRegister shaderClass##_register( \
        ShaderDescriptorHandle( filePath ), \
        filePath, \
        shaderClass::Type, \
        entryPoint, \
        +[]() -> std::shared_ptr<IShaderPermutation> \
        { \
            if constexpr ( HasShaderPermutationType<shaderClass> ) \
            { \
                return std::make_shared<typename shaderClass::PermutationType>(); \
            } \
            else \
            { \
                return std::make_shared<ShaderPermutation<>>(); \
            } \
        } \
    )
}
