#include "GlobalShaders.h"

#include "AssetLoader.h"
#include "TaskScheduler.h"

#include <cassert>
#include <memory>

void GlobalShader::BootUp( )
{
	IAssetLoader* assetLoader = GetInterface<IAssetLoader>( );
	if ( assetLoader == nullptr )
	{
		return;
	}

	for ( const auto& pathPair : m_shaderAssetPaths )
	{
		auto[typeIndex, assetPath] = pathPair;

		if ( m_shaders.find( typeIndex ) != std::end( m_shaders ) )
		{
			continue;
		}

		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunctor( [typeIndex]( const std::shared_ptr<void>& asset )
		{
			GlobalShader::GetInstance( ).RegisterShader( typeIndex, std::static_pointer_cast<ShaderBase>( asset ) );
		} );

		AssetLoaderSharedHandle handle = assetLoader->RequestAsyncLoad( assetPath, onLoadComplete );

		assert( handle->IsLoadingInProgress( ) || handle->IsLoadComplete( ) );
		++m_loadingInProgress;
	}
}

bool GlobalShader::RegisterShader( std::type_index typeIndex, const std::shared_ptr<ShaderBase>& shader )
{
	auto found = m_shaders.find( typeIndex );
	assert( found == std::end( m_shaders ) );

	m_shaders[typeIndex] = shader;
	--m_loadingInProgress;

	return true;
}

bool GlobalShader::RegisterShaderPath( std::type_index typeIndex, const char* path )
{
	auto found = m_shaderAssetPaths.find( typeIndex );
	if ( found != std::end( m_shaderAssetPaths ) )
	{
		return false;
	}

	m_shaderAssetPaths[typeIndex] = path;
	return true;
}

ShaderBase* GlobalShader::GetShader( std::type_index typeIndex )
{
	auto found = m_shaders.find( typeIndex );
	if ( found == std::end( m_shaders ) )
	{
		return nullptr;
	}

	return &( *found->second );
}

GlobalShaderRegister::GlobalShaderRegister( std::type_index typeIndex, const char* assetPath )
{
	assert( IsInGameThread( ) );

	GlobalShader::GetInstance( ).RegisterShaderPath( typeIndex, assetPath );
}

ShaderBase* GetGlobalShaderImpl( std::type_index typeIndex )
{
	return GlobalShader::GetInstance( ).GetShader( typeIndex );
}
