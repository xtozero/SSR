#pragma once

#include "AssetFactory.h"
#include "IAsyncLoadableAsset.h"
#include "Shader.h"
#include "SizedTypes.h"

#include <map>
#include <memory>

namespace rendercore
{
	class ShaderCache final : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( ShaderCache );
		DECLARE_ASSET( RENDERCORE, ShaderCache );

	public:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

		static bool IsLoaded();
		static void LoadFromFile();
		static void OnLoadFinished( const std::shared_ptr<void>& shaderCache );
		static ShaderBase* GetCachedShader( uint64 shaderHash );
		static void UpdateCache( uint64 shaderHash, ShaderBase* shader );
		static void Shutdown();

	private:
		static void SaveToFile();

		static std::shared_ptr<ShaderCache> m_shaderCache;

		PROPERTY( shaders )
		std::map<uint64, std::unique_ptr<ShaderBase>> m_shaders;
	};
}
