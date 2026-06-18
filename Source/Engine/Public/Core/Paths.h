#pragma once

#include "Common.h"

#include <filesystem>

namespace engine
{
    class Paths
    {
    public:
        ENGINE_DLL static std::filesystem::path GetShaderSourceRootDir();
        ENGINE_DLL static std::filesystem::path GetShaderAssetRootDir();
        ENGINE_DLL static std::filesystem::path GetSaveDir();
    };
}
