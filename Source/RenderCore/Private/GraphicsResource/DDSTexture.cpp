#include "DDSTexture.h"

#include "ArchiveUtility.h"

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

	void Texture::PostLoadImpl()
	{
		// Do nothing
	}

	REGISTER_ASSET( DDSTexture );
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
		agl::ResourceMisc misc = agl::ResourceMisc::None;
		misc |= m_isCubeMap ? agl::ResourceMisc::TextureCube : agl::ResourceMisc::None;
		misc |= ( m_depth > 1 ) ? agl::ResourceMisc::Texture3D : agl::ResourceMisc::None;

		agl::TextureTrait tarit = {
			.m_width = m_width,
			.m_height = m_height,
			.m_depth = m_depth * m_arraySize,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = m_mipLevels,
			.m_format = m_format,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = misc
		};

		agl::ResourceInitData initData;
		initData.m_sections.resize( m_sections.size() );
		initData.m_srcData = m_memory.Data();
		initData.m_srcSize = m_memory.Size();

		for ( size_t i = 0; i < initData.m_sections.size(); ++i )
		{
			initData.m_sections[i].m_offset = m_sections[i].m_offset;
			initData.m_sections[i].m_pitch = m_sections[i].m_rowPitch;
			initData.m_sections[i].m_slicePitch = m_sections[i].m_slicePitch;
		}

		m_texture = agl::Texture::Create( tarit, Path().generic_string().c_str(), &initData);

		EnqueueRenderTask(
			[texture = m_texture]()
			{
				texture->Init();
			} );
	}
}