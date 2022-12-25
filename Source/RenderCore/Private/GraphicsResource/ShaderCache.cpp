#include "ShaderCache.h"

#include "IAgl.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace
{
	const char* GetShaderCachePath()
	{
		agl::AglType type = GetInterface<agl::IAgl>()->GetType();
		switch ( type )
		{
		case agl::AglType::D3D11:
			return "./Assets/Shaders/ShaderCache.asset";
			break;
		case agl::AglType::D3D12:
			return "./Assets/Shaders/ShaderCache-d3d12.asset";
			break;
		}
	}
}

namespace rendercore
{
	std::shared_ptr<ShaderCache> ShaderCache::m_shaderCache;

	REGISTER_ASSET( ShaderCache );
	bool ShaderCache::IsLoaded()
	{
		return m_shaderCache != nullptr;
	}

	void ShaderCache::LoadFromFile()
	{
		const std::string cachePath = GetShaderCachePath();

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

			ar.WriteToFile( GetShaderCachePath() );
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
}
