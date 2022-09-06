#include "ShaderCache.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace
{
	static const std::string cachePath = "./Assets/Shaders/ShaderCache.asset";
}

std::shared_ptr<ShaderCache> ShaderCache::m_shaderCache;

REGISTER_ASSET( ShaderCache );
bool ShaderCache::IsLoaded()
{
	return m_shaderCache != nullptr;
}

void ShaderCache::LoadFromFile()
{
	if ( fs::exists( cachePath ) )
	{
		IAssetLoader::LoadCompletionCallback onLoadComplete;
		onLoadComplete.BindFunction( &ShaderCache::OnLoadFinished );

		GetInterface<IAssetLoader>()->RequestAsyncLoad( cachePath, onLoadComplete );
	}
	else
	{
		m_shaderCache = std::make_shared<ShaderCache>();
	}
}

void ShaderCache::SaveToFile()
{
	if ( m_shaderCache != nullptr )
	{
		Archive ar;
		m_shaderCache->Serialize( ar );

		ar.WriteToFile( cachePath );
	}
}

void ShaderCache::OnLoadFinished( const std::shared_ptr<void>& shaderCache )
{
	m_shaderCache = std::reinterpret_pointer_cast<ShaderCache>( shaderCache );
}

ShaderBase* ShaderCache::GetCachedShader( uint64 shaderHash )
{
	if ( m_shaderCache == nullptr )
	{
		return nullptr;
	}

	auto found = m_shaderCache->m_shaders.find( shaderHash );
	if ( found == std::end( m_shaderCache->m_shaders ) )
	{
		return nullptr;
	}
	
	return found->second.get();
}

void ShaderCache::UpdateCache( uint64 shaderHash, ShaderBase* shader )
{
	if ( m_shaderCache == nullptr )
	{
		return;
	}

	m_shaderCache->m_shaders[shaderHash].reset( shader );
}

void ShaderCache::PostLoadImpl()
{
	for ( auto& [key, shader] : m_shaders )
	{
		shader->CreateShader();
	}
}
