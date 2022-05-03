#pragma once

#include "AssetFactory.h"
#include "GraphicsApiResource.h"
#include "IAsyncLoadableAsset.h"
#include "SizedTypes.h"
#include "Texture.h"

#include <vector>

struct TextureSection
{
	TextureSection( uint32 rowPitch, uint32 slicePitch, uint32 offset ) : m_rowPitch( rowPitch ), m_slicePitch( slicePitch ), m_offset( offset )
	{}
	TextureSection() = default;

	uint32 m_rowPitch;
	uint32 m_slicePitch;
	uint32 m_offset;
};

class Texture : public AsyncLoadableAsset
{
	GENERATE_CLASS_TYPE_INFO( Texture );
	DECLARE_ASSET( RENDERCORE, Texture );

public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	aga::Texture* Resource();
	const aga::Texture* Resource() const;

protected:
	RENDERCORE_DLL virtual void PostLoadImpl() override;

	uint32 m_width = 0;
	uint32 m_height = 0;
	uint32 m_depth = 0;
	uint32 m_arraySize = 0;
	uint32 m_mipLevels = 0;

	bool m_isCubeMap = false;
	uint32 m_demension = 0;

	RESOURCE_FORMAT m_format = RESOURCE_FORMAT::UNKNOWN;

	BinaryChunk m_memory;
	std::vector<TextureSection> m_sections;

	RefHandle<aga::Texture> m_texture;
};

struct DDSTextureInitializer;

class DDSTexture : public Texture
{
	GENERATE_CLASS_TYPE_INFO( DDSTexture );
	DECLARE_ASSET( RENDERCORE, DDSTexture );

public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	DDSTexture() = default;
	RENDERCORE_DLL explicit DDSTexture( const DDSTextureInitializer& initializer );

protected:
	RENDERCORE_DLL virtual void PostLoadImpl() override;
};

struct DDSTextureInitializer
{
	uint32 m_width = 0;
	uint32 m_height = 0;
	uint32 m_depth = 0;
	uint32 m_arraySize = 0;
	uint32 m_mipLevels = 0;

	bool m_isCubeMap = false;
	uint32 m_demension = 0;

	RESOURCE_FORMAT m_format;

	uint32 m_size = 0;
	const uint8* m_memory = nullptr;

	std::vector<TextureSection> m_sections;
};
