#include "GlobalShaders.h"

#include "AssetLoader/AssetLoader.h"
#include "MultiThread/EngineTaskScheduler.h"

#include <cassert>
#include <memory>

bool GlobalShader::RegisterShader( std::type_index typeIndex, const std::shared_ptr<ShaderBase>& shader )
{
	auto found = m_shaders.find( typeIndex );
	if ( found == std::end( m_shaders ) )
	{
		return false;
	}

	if ( found->second != nullptr )
	{
		return false;
	}

	found->second = shader;
	--m_loadingInProgress;

	return true;
}

void GlobalShader::OnLoadShaderStarted( std::type_index typeIndex )
{
	auto result = m_shaders.emplace( typeIndex, nullptr );
	assert( result.second );
	++m_loadingInProgress;
}

ShaderBase* GlobalShader::GetShader( std::type_index typeIndex )
{
	assert( m_loadingInProgress <= 0 );
	auto found = m_shaders.find( typeIndex );
	if ( found == std::end( m_shaders ) )
	{
		return nullptr;
	}

	return &( *found->second );
}

GlobalShaderRegister::GlobalShaderRegister( std::type_index typeIndex, const std::string& assetPath )
{
	assert( IsInGameThread( ) );

	IAssetLoader* assetLoader = GetInterface<IAssetLoader>( );
	if ( assetLoader == nullptr )
	{
		return;
	}

	if ( assetPath.length( ) > 0 )
	{
		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunctor( [typeIndex]( const std::shared_ptr<void>& asset )
		{
			GlobalShader::GetInstance( ).RegisterShader( typeIndex, std::static_pointer_cast<ShaderBase>( asset ) );
		} );

		AssetLoaderSharedHandle handle = assetLoader->RequestAsyncLoad( assetPath, onLoadComplete );

		assert( handle->IsLoadingInProgress( ) || handle->IsLoadComplete( ) );
		if ( handle->IsLoadingInProgress( ) || handle->IsLoadComplete( ) )
		{
			GlobalShader::GetInstance( ).OnLoadShaderStarted( typeIndex );
		}
	}
}

ShaderBase* GetGlobalShaderImpl( std::type_index typeIndex )
{
	return GlobalShader::GetInstance( ).GetShader( typeIndex );
}
