#pragma once

#include "Archive.h"
#include "AssetFactory.h"
#include "IAsyncLoadableAsset.h"

#include <memory>

namespace rendercore
{
	enum class PipelineStateCacheType
	{
		HandmadeCache,
		LibraryCache,
	};

	class PipelineStateCache final : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( PipelineStateCache );
		DECLARE_ASSET( RENDERCORE, PipelineStateCache );

	public:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

		static bool IsLoaded();
		static void LoadFromFile();
		static void OnLoadFinished( const std::shared_ptr<void>& psoCache );
		static void Shutdown();

	private:
		static void SaveToFile();

		static std::shared_ptr<PipelineStateCache> m_pipelineStateCache;

		PROPERTY( psoCacheType )
		PipelineStateCacheType m_psoCacheType = PipelineStateCacheType::HandmadeCache;

		PROPERTY( psoCache )
		std::map<uint64, BinaryChunk> m_psoCache;

		PROPERTY( psoLibraryCache )
		BinaryChunk m_psoLibraryCache;
	};
}