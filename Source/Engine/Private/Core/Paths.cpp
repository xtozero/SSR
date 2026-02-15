#include "Paths.h"

namespace engine
{
    std::filesystem::path Paths::GetShaderSourceRootDir()
    {
        static auto path = std::filesystem::path( "../Source/Shaders/Private" ).make_preferred();
        return path;
    }

    std::filesystem::path Paths::GetShaderAssetRootDir()
    {
        static auto path = std::filesystem::path( "../Program/Assets/Shaders" ).make_preferred();
        return path;
    }
}
