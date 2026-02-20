#include "GlobalShaders.h"

#include "AssetLoader.h"
#include "Core/Paths.h"
#include "Platform/CommandLine.h"
#include "TaskScheduler.h"

#include <cassert>
#include <memory>

namespace rendercore
{
	void GlobalShaders::BootUp()
	{
		IAssetLoader* assetLoader = GetInterface<IAssetLoader>();
		if ( assetLoader == nullptr )
		{
			return;
		}

		bool isRunningAssetBuilder = engine::CommandLine::Has( StaticName( "AssetBuilder" ) );
		for ( auto& [typeIndex, assetPath] : m_shaderAssetPaths )
		{
			if ( m_shaders.contains( typeIndex ) )
			{
				continue;
			}

			if ( isRunningAssetBuilder && ( std::filesystem::exists( assetPath ) == false ) )
			{
				continue;
			}

			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindFunctor( [typeIndex]( const std::shared_ptr<void>& asset )
				{
					EnqueueRenderTask(
						[typeIndex, asset]()
						{
							GlobalShaders::GetInstance().RegisterShader( typeIndex, std::static_pointer_cast<ShaderAsset>( asset ) );
						} );
				} );

			AssetLoaderSharedHandle handle = assetLoader->RequestAsyncLoad( assetPath.generic_string(), onLoadComplete );

			assert( handle->IsLoadingInProgress() || handle->IsLoadComplete() );
			++m_loadingInProgress;
		}
	}

	void GlobalShaders::Shutdown()
	{
		m_shaders.clear();
	}

	bool GlobalShaders::IsReady() const
	{
		return m_loadingInProgress == 0;
	}

	void GlobalShaders::Reload()
	{
		assert( IsReady() );
		Shutdown();
		BootUp();
	}

	bool GlobalShaders::RegisterShader( std::type_index typeIndex, const std::shared_ptr<ShaderAsset>& shader )
	{
		assert( m_shaders.contains( typeIndex ) == false );

		m_shaders[typeIndex] = shader;
		--m_loadingInProgress;

		return true;
	}

	bool GlobalShaders::RegisterShaderPath( std::type_index typeIndex, agl::ShaderType shaderType, const char* path )
	{
		if ( m_shaderAssetPaths.contains( typeIndex ) )
		{
			return false;
		}

		auto assetName = GetShaderAssetName( path, shaderType );

		auto assetPath = ( engine::Paths::GetShaderAssetRootDir() / path );
		assetPath.replace_filename( assetName ).replace_extension(".asset").make_preferred();

		m_shaderAssetPaths[typeIndex] = assetPath;
		return true;
	}

	ShaderAsset* GlobalShaders::GetShader( std::type_index typeIndex )
	{
		auto found = m_shaders.find( typeIndex );
		if ( found == std::end( m_shaders ) )
		{
			return nullptr;
		}

		return &( *found->second );
	}

	GlobalShaderRegister::GlobalShaderRegister( std::type_index typeIndex, agl::ShaderType shaderType, const char* assetPath )
	{
		GlobalShaders::GetInstance().RegisterShaderPath( typeIndex, shaderType, assetPath );
	}

	ShaderAsset* GetGlobalShaderImpl( std::type_index typeIndex )
	{
		assert( IsInRenderThread() );
		return GlobalShaders::GetInstance().GetShader( typeIndex );
	}
}
