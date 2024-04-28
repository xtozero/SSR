#include "PipelineStateCache.h"

#include "IAgl.h"
#include "IRenderResourceManager.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace rendercore
{
	std::shared_ptr<PipelineStateCache> PipelineStateCache::m_pipelineStateCache;

	REGISTER_ASSET( PipelineStateCache );
	void PipelineStateCache::PostLoadImpl()
	{
	}

	bool PipelineStateCache::IsLoaded()
	{
		auto agl = GetInterface<agl::IAgl>();
		return ( m_pipelineStateCache != nullptr ) || ( agl->IsSupportsPSOCache() == false );
	}

	void PipelineStateCache::LoadFromFile()
	{
		auto agl = GetInterface<agl::IAgl>();
		if ( agl->IsSupportsPSOCache() == false )
		{
			return;
		}

		const fs::path cachePath = agl->GetPSOCacheFilePath();
		if ( fs::exists( cachePath ) )
		{
			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindFunction( &PipelineStateCache::OnLoadFinished );

			GetInterface<IAssetLoader>()->RequestAsyncLoad( cachePath.generic_string(), onLoadComplete);
		}
		else
		{
			OnLoadFinished( std::make_shared<PipelineStateCache>() );
		}
	}

	void PipelineStateCache::OnLoadFinished( const std::shared_ptr<void>& psoCache )
	{
		if ( psoCache == nullptr )
		{
			return;
		}

		m_pipelineStateCache = std::reinterpret_pointer_cast<PipelineStateCache>( psoCache );
		std::map<uint64, BinaryChunk>& psoCacheRef = m_pipelineStateCache->m_psoCache;

		EnqueueRenderTask(
			[&psoCacheRef]()
			{
				GetInterface<agl::IResourceManager>()->SetPSOCache( psoCacheRef );
			} );
	}

	void PipelineStateCache::Shutdown()
	{
		SaveToFile();

		if ( m_pipelineStateCache == nullptr )
		{
			return;
		}

		m_pipelineStateCache->m_psoCache.clear();
		m_pipelineStateCache = nullptr;
	}

	void PipelineStateCache::SaveToFile()
	{
		auto agl = GetInterface<agl::IAgl>();
		if ( agl->IsSupportsPSOCache() == false )
		{
			return;
		}

		if ( m_pipelineStateCache != nullptr )
		{
			Archive ar;
			m_pipelineStateCache->Serialize( ar );

			const fs::path cachePath = agl->GetPSOCacheFilePath();
			ar.WriteToFile( cachePath );
		}
	}
}
