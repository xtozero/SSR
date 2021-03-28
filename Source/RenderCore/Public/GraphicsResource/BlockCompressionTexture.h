#pragma once

#include "AssetLoader/AssetFactory.h"
#include "AssetLoader/IAsyncLoadableAsset.h"
#include "GraphicsApiResource.h"
#include "Texture.h"

class Texture : public AsyncLoadableAsset
{
	DECLARE_ASSET( RENDERCORE, Texture );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
};

class BCTexture : public Texture
{
	DECLARE_ASSET( RENDERCORE, BCTexture );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
};