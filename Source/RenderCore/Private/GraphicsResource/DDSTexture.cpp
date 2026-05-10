#include "DDSTexture.h"

#include "ArchiveUtility.h"

#include <utility>

namespace
{
	std::pair<uint32, uint32> CalcTextureSize( uint32 width, uint32 height, uint32 mip )
	{
		width = std::max<uint32>( width >> mip, 1 );
		height = std::max<uint32>( height >> mip, 1 );

		return std::make_pair( width, height );
	}
}

namespace rendercore
{
	Archive& operator<<( Archive& ar, TextureSection& section )
	{
		ar << section.m_rowPitch << section.m_slicePitch << section.m_offset;

		return ar;
	}

	REGISTER_ASSET( Texture );
	agl::Texture* Texture::Resource()
	{
		return m_texture.Get();
	}

	const agl::Texture* Texture::Resource() const
	{
		return m_texture.Get();
	}

	uint32 Texture::GetWidth() const
	{
		return m_width;
	}

	uint32 Texture::GetHeight() const
	{
		return m_height;
	}

	uint32 Texture::GetMipLevels() const
	{
		return m_mipLevels;
	}

	int32 Texture::ClampMipLevel( int32 mipLevel ) const
	{
		return std::clamp<int32>( mipLevel, 0, GetMipLevels() - 1 );
	}

	REGISTER_ASSET( DDSTexture );
	void DDSTexture::RequestMipLevels( int32 desiredMipLevel )
	{
		desiredMipLevel = ClampMipLevel( desiredMipLevel );
		if ( m_desiredMipLevel == desiredMipLevel )
		{
			return;
		}

		m_desiredMipLevel = desiredMipLevel;

		agl::ResourceInitData initData = GetInitData();

		const auto [width, height] = CalcTextureSize( m_width, m_height, m_desiredMipLevel );
		uint32 mipLevels = m_mipLevels - m_desiredMipLevel;

		EnqueueRenderTask(
			[texture = m_texture, width, height, mipLevels, initData]()
			{
				texture->UpdateTextureMips( width, height, mipLevels, initData );
			} );
	}

	DDSTexture::DDSTexture( const DDSTextureDesc& desc )
	{
		m_width = desc.m_width;
		m_height = desc.m_height;
		m_depth = desc.m_depth;
		m_arraySize = desc.m_arraySize;
		m_mipLevels = desc.m_mipLevels;

		m_isCubeMap = desc.m_isCubeMap;
		m_demension = desc.m_demension;

		m_format = desc.m_format;

		std::construct_at( &m_memory, desc.m_size );
		std::memcpy( m_memory.Data(), desc.m_memory, desc.m_size );

		m_sections = desc.m_sections;
	}

	void DDSTexture::PostLoadImpl()
	{
		m_desiredMipLevel = ClampMipLevel( m_mipLevels - 1 );

		CreateRenderResource();
	}

	agl::ResourceInitData DDSTexture::GetInitData()
	{
		uint32 mipLevels = m_mipLevels - m_desiredMipLevel;
		uint32 depth = m_depth * m_arraySize;

		const size_t firstSectionIndex = m_desiredMipLevel * depth;
		const size_t firstSectionOffset = m_sections[firstSectionIndex].m_offset;

		agl::ResourceInitData initData;
		initData.m_sections.resize( mipLevels * depth );
		initData.m_srcData = m_memory.Data() + firstSectionOffset;
		initData.m_srcSize = m_memory.Size() - firstSectionOffset;

		for ( size_t i = 0; i < initData.m_sections.size(); ++i )
		{
			const size_t sectionIndex = firstSectionIndex + i;

			initData.m_sections[i].m_offset = m_sections[sectionIndex].m_offset - firstSectionOffset;
			initData.m_sections[i].m_pitch = m_sections[sectionIndex].m_rowPitch;
			initData.m_sections[i].m_slicePitch = m_sections[sectionIndex].m_slicePitch;
		}

		return initData;
	}

	void DDSTexture::CreateRenderResource()
	{
		uint32 mipLevels = m_mipLevels - m_desiredMipLevel;

		agl::ResourceMisc misc = agl::ResourceMisc::None;
		misc |= m_isCubeMap ? agl::ResourceMisc::TextureCube : agl::ResourceMisc::None;
		misc |= ( m_depth > 1 ) ? agl::ResourceMisc::Texture3D : agl::ResourceMisc::None;

		const auto [width, height] = CalcTextureSize( m_width, m_height, m_desiredMipLevel );
		uint32 depth = m_depth * m_arraySize;

		agl::TextureDesc tarit = {
			.m_width = width,
			.m_height = height,
			.m_depth = depth,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = mipLevels,
			.m_format = m_format,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = misc
		};

		agl::ResourceInitData initData = GetInitData();
		m_texture = agl::Texture::Create( tarit, Path().generic_string().c_str(), &initData );
	}
}
