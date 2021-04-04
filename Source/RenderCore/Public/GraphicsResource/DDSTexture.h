#pragma once

#include "AssetLoader/AssetFactory.h"
#include "AssetLoader/IAsyncLoadableAsset.h"
#include "GraphicsApiResource.h"
#include "Texture.h"

#include <vector>

class Texture : public AsyncLoadableAsset
{
	DECLARE_ASSET( RENDERCORE, Texture );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

	std::size_t m_width = 0;
	std::size_t m_height = 0;
	std::size_t m_depth = 0;
	std::size_t m_arraySize = 0;
	std::size_t m_mipLevels = 0;

	bool m_isCubeMap = false;
	std::size_t m_demension = 0;

	RESOURCE_FORMAT m_format = RESOURCE_FORMAT::UNKNOWN;

	BinaryChunk m_memory;

private:
	std::filesystem::path m_path;
};

struct DDSTextureInitializer;

class DDSTexture : public Texture
{
	DECLARE_ASSET( RENDERCORE, DDSTexture );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	DDSTexture( ) = default;
	RENDERCORE_DLL explicit DDSTexture( const DDSTextureInitializer& initializer );

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
};

struct DDSTextureInitializer
{
	std::size_t m_width = 0;
	std::size_t m_height = 0;
	std::size_t m_depth = 0;
	std::size_t m_arraySize = 0;
	std::size_t m_mipLevels = 0;

	bool m_isCubeMap = false;
	std::size_t m_demension = 0;

	RESOURCE_FORMAT m_format;

	std::size_t m_size = 0;
	const void* m_memory = nullptr;
};
