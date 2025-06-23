#include "AssetBuilderLauncher.h"

#include "PlatformProcess.h"

namespace fs = std::filesystem;

namespace engine
{
    void RunAssetBuilder()
    {
        static const fs::path executableName = "AssetBuilder.exe";
        fs::path assetBuilderPath = PlatformProcess::GetExecutableDirectory() / executableName;

        static const fs::path workingFolderName = "AssetBuilder";
        fs::path workingDirectory = PlatformProcess::GetRootDirectory() / workingFolderName;

        PlatformProcess::LaunchApplication( "open",
            assetBuilderPath.generic_string().c_str(),
            nullptr,
            workingDirectory.generic_string().c_str(),
            false );
    }
}
