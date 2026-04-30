#include "DirectXTexTool.h"

#include "DirectXTex.h"
#include "../DXGI/DxgiFlagConvertor.h"

uint32 ConvertToBCTextureDemension( DirectX::TEX_DIMENSION dimension )
{
	switch ( dimension )
	{
	case DirectX::TEX_DIMENSION_TEXTURE1D:
		return 1;
		break;
	case DirectX::TEX_DIMENSION_TEXTURE2D:
		return 2;
		break;
	case DirectX::TEX_DIMENSION_TEXTURE3D:
		return 3;
		break;
	default:
		break;
	}

	return 0;
}

rendercore::DDSTextureDesc ConvertToBCTextureDesc( const DirectX::ScratchImage& image )
{
	rendercore::DDSTextureDesc desc;

	const DirectX::TexMetadata& meta = image.GetMetadata();
	desc.m_width = static_cast<uint32>( meta.width );
	desc.m_height = static_cast<uint32>( meta.height );
	desc.m_depth = static_cast<uint32>( meta.depth );
	desc.m_arraySize = static_cast<uint32>( meta.arraySize );
	desc.m_mipLevels = static_cast<uint32>( meta.mipLevels );

	desc.m_isCubeMap = meta.IsCubemap();
	desc.m_demension = ConvertToBCTextureDemension( meta.dimension );

	desc.m_format = agl::ConvertDxgiFormatToFormat( meta.format );

	desc.m_size = static_cast<uint32>( image.GetPixelsSize() );
	desc.m_memory = image.GetPixels();

	for ( size_t mip = 0; mip < meta.mipLevels; ++mip )
	{
		for ( size_t item = 0; item < meta.arraySize; ++item )
		{
			size_t tIdx = meta.ComputeIndex( mip, item, 0 );
			auto subresources = image.GetImages();

			auto rowPitch = static_cast<uint32>( subresources[tIdx].rowPitch );
			auto slicePitch = static_cast<uint32>( subresources[tIdx].slicePitch );
			auto offset = static_cast<uint32>( subresources[tIdx].pixels - desc.m_memory );

			desc.m_sections.emplace_back( rowPitch, slicePitch, offset );
		}
	}

	return desc;
}