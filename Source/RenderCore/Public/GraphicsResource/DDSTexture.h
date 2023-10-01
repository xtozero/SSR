#pragma once

#include "AssetFactory.h"
#include "GraphicsApiResource.h"
#include "IAsyncLoadableAsset.h"
#include "SizedTypes.h"
#include "Texture.h"

#include <vector>

namespace rendercore
{
	struct TextureSection
	{
		TextureSection( uint32 rowPitch, uint32 slicePitch, uint32 offset ) : m_rowPitch( rowPitch ), m_slicePitch( slicePitch ), m_offset( offset )
		{}
		TextureSection() = default;

		uint32 m_rowPitch;
		uint32 m_slicePitch;
		uint32 m_offset;

		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, TextureSection& section );
	};

	class Texture : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( Texture );
		DECLARE_ASSET( RENDERCORE, Texture );

	public:
		agl::Texture* Resource();
		const agl::Texture* Resource() const;

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

		PROPERTY( width )
		uint32 m_width = 0;

		PROPERTY( height )
		uint32 m_height = 0;

		PROPERTY( depth )
		uint32 m_depth = 0;

		PROPERTY( arraySize )
		uint32 m_arraySize = 0;

		PROPERTY( mipLevels )
		uint32 m_mipLevels = 0;

		PROPERTY( isCubeMap )
		bool m_isCubeMap = false;

		PROPERTY( demension )
		uint32 m_demension = 0;

		PROPERTY( format )
		agl::ResourceFormat m_format = agl::ResourceFormat::Unknown;

		PROPERTY( memory )
		BinaryChunk m_memory;

		PROPERTY( sections )
		std::vector<TextureSection> m_sections;

		agl::RefHandle<agl::Texture> m_texture;
	};

	struct DDSTextureInitializer;

	class DDSTexture : public Texture
	{
		GENERATE_CLASS_TYPE_INFO( DDSTexture );
		DECLARE_ASSET( RENDERCORE, DDSTexture );

	public:
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

		agl::ResourceFormat m_format = agl::ResourceFormat::Unknown;

		uint32 m_size = 0;
		const uint8* m_memory = nullptr;

		std::vector<TextureSection> m_sections;
	};
}